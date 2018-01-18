
function last(ary) {
    if (ary.length === 0) {
        return null;
    } else {
        return ary[ary.length - 1];
    }
}

Array.prototype.last = function () {
    return this[this.length -1];
}

function VDCEUI(host, port) {

    var _serverSocket = new WebSocket("ws://" + host + ":" + port);
    var _restURL = "http://" + host + ":" + port;
    var _network = null;
    var _networks = null;
    var _sim = { run: false, i: 0, n: null, t: null, cb: null, rm: true };

    var _data = {
        server_util: []
    };

    getTopology();

    _serverSocket.onopen = function (event) {
        console.log("connected");
        // retrieve topology
    };

    _serverSocket.onerror = function(error) {
        console.log("error: " + error);
    };

    _serverSocket.onmessage = function(event) {
        var message = JSON.parse(event.data);


        if (message.msg === 'success') {
            $(".footer").html("success");
            setTimeout(function() { $(".footer").html(""); }, 1500);
            getUtilization();
            getNetworks();

            if (_sim.i > 0)
                continueSim();


        } else if(message.msg === 'error') {
            $(".footer").html("error");
            setTimeout(function() { $(".footer").html(""); }, 1500);
            getUtilization();

            if (_sim.i > 0)
                continueSim();
        }
    };

    function getTopology() {
        $.getJSON(_restURL + "/physical_network.json", function(data) {
            console.log("GET " + _restURL + "/physical_network.json");
            _network = {};
            _network.links = data.links;
            _network.nodes = data.nodes;

            $(".physical.statistic .value").html(data.nodes.length + " / " + data.links.length)

            drawNetwork(data);
            getUtilization();
        });
    }

    function getUtilization() {

        $.getJSON(_restURL + "/utilization.json", function(data) {
            console.log("GET " + _restURL + "/utilization.json");

            $.each(data.nodes, function(k, v) {
                _network.nodes[k].utilization = v;
                _setNodeUtilization(k, v[0], _network.nodes[k].type);
            });

            $.each(data.links, function(k, v) {
                _network.links[k].utilization = v;
                _setLinkUtilization(k, v);
            });

            if (data.virtual_networks > 0) {
                $(".statistics > .virtual.statistic > .value").html(data.virtual_networks);
                $(".statistics > .server_utilization.statistic > .value").html(data.server_util.last().toFixed(2));
                $(".statistics > .switch_utilization.statistic > .value").html(data.switch_util.last().toFixed(2));
                $(".statistics > .edge_utilization.statistic > .value").html(data.edge_util.last().toFixed(2));
                $(".statistics > .accept.statistic > .value").html(data.accept.last().toFixed(2));
                $(".statistics > .server_var.statistic > .value").html(data.server_var.last().toFixed(2));
                $(".statistics > .switch_var.statistic > .value").html(data.switch_var.last().toFixed(2));
                $(".statistics > .edge_var.statistic > .value").html(data.edge_var.last().toFixed(2));
                $(".statistics > .path_len.statistic > .value").html(data.path_len.last().toFixed(2));
            }

            drawLineChart(data);
        });
    }

    function getNetworks() {

        $.getJSON(_restURL + "/vnets", function(data) {
            var template = Handlebars.compile($("#main-networks-template").html());
            $(".ui.tab[data-tab='main-networks']").html(template(data));

            _networks = data.networks;
        });
    }

    function requestVN(net) {
        console.log("POST " + _restURL + "/vnets");

        $.post(_restURL + "/vnets", JSON.stringify(net), function(result) {

            if (result.success) {
                $(".footer").html("success");
                setTimeout(function() { $(".footer").html(""); }, 1500);
                getNetworks();
            } else  {
                $(".footer").html("error");
                setTimeout(function() { $(".footer").html(""); }, 1500);
                getUtilization();
            }

            if (_sim.i > 0)
                continueSim();

            getUtilization();

        }, "json")
            .done(function() {
                console.log("xhr done");
            })
            .fail(function() {
                console.log("xhr fail");
            });
    }

    function deleteVN(net_id) {
        console.log("DELETE " + _restURL + "/vnets/" + net_id);
        $.ajax({
            url: _restURL + "/vnets/" + net_id,
            type: 'DELETE',
            success: function(result) {
                getUtilization();
                getNetworks();
                if (_sim.i > 0)
                    continueSim();
            }
        });
    }

    function remap() {
        console.log("POST " + _restURL + "/vnets/remap");
        $.post({
            url: _restURL + "/vnets/remap",
            success: function(result) {
                getUtilization();
                getNetworks();
                alert("done");
            }
        });
    }

    function randomVN() {
        return _networks[Math.floor(Math.random() * _networks.length)].id;
    }

    function startSim(n, type, cb, rm) {
        _sim.n = n;
        _sim.t = type;
        _sim.i = 0;
        _sim.cb = cb;
        _sim.rm = rm;

        continueSim();
    }

    function continueSim() {

        if (_sim.i < _sim.n) {

            if (_sim.rm) {

                var i = Math.floor(Math.random() * 10)

                if (i >= 7) {
                    deleteVN(randomVN());
                } else {
                    requestVN(_sim.cb(_sim.t));
                }
            } else {
                requestVN(_sim.cb(_sim.t));
            }

            _sim.i++;
        } else {
            stopSim();
        }
    }

    function stopSim() {

        _sim.n = null;
        _sim.t = null;
        _sim.i = 0;

        alert("done");
    }

    function drawLineChart(data) {

        var formattedData = {

            series: [
                data.server_util,
                data.switch_util,
                data.edge_util,
                data.server_var,
                data.switch_var,
                data.edge_var,
                data.accept,
                data.path_len
            ]
        };

        var options = {
            width: 445,
            height: 230,
            axisX: {
                showGrid: false,
                showLabel: false,
                offset: 0
            },
            axisY: {
                offset: 30
            },
            fullWidth: true,
            lineSmooth: Chartist.Interpolation.none(),
            showPoint: false
        };

        new Chartist.Line('.ct-chart', formattedData, options);
    }

    function drawNetwork(data) {

        function _renderThreshold(simulation) {
            return Math.ceil(Math.log(simulation.alphaMin()) / Math.log(1 - simulation.alphaDecay()));
        }

        var svg = d3.select("svg.topo");
        var width = svg.attr("width");
        var height = svg.attr("height");

        var g = svg.append("g");
        g.attr("transform", "translate(" + width / 2 + "," + height / 2 + ")");

        var nodes = data.nodes;
        var links = data.links;
        var n = Object.keys(nodes).length;

        $.each(links, function(k, v) {
            links[k] = {"source": v["from"], "target": v["to"]};
        });

        var scale = 132;

        var simulation = d3.forceSimulation(nodes)
            .force("charge", d3.forceManyBody().strength(-(3*scale)))
            .force("link", d3.forceLink(links).distance(scale).strength(1).iterations(50))
            .force("x", d3.forceX())
            .force("y", d3.forceY())
            .stop();

        for (var i = 0, n = _renderThreshold(simulation); i < n; ++i) {
            simulation.tick();
        }

        g.append("g")
            .attr("stroke", "#bbb").attr("stroke-width", 1)
            .selectAll("line")
            .data(links)
            .enter().append("line")
            .attr("id", function(d) { return "vdce_link_" + d.index })
            .attr("x1", function(d) { return d.source.x; }).attr("y1", function(d) { return d.source.y; })
            .attr("x2", function(d) { return d.target.x; }).attr("y2", function(d) { return d.target.y; })
            .on("mouseover", _mouseOverLink)
            .on("mouseout", _mouseOutLink)
            .on("click", _clickLink);

        g.append("g")
            .attr("stroke-width", 0.5).attr("stroke", "#bbb").style("fill", "#ffffff").attr("r", 2)
            .selectAll("circle")
            .data(nodes)
            .enter().append("circle")
            .attr("id", function(d) { return "vdce_node_" + d.id })
            .attr("cx", function(d) { return d.x; }).attr("cy", function(d) { return d.y; })
            .on("mouseover", _mouseOverNode)
            .on("mouseout", _mouseOutNode)
            .on("click", _clickNode);
    }

    function _linkColor(value) {
        gray = ['#bbbbbb','#a9a9a9','#969696','#838383','#727272','#616161','#515151','#404040','#303030','#222222'];
        return d3.rgb(gray[Math.round(value * (10-1))]);
    }

    function _nodeColor(type, value) {
        blue = ['#ffffff','#e4e5ed','#c9cddc','#afb5ca','#959db9','#7c87a8','#627097','#485b86','#2c4776','#003366'];
        red =  ['#ffffff','#f1e2df','#e3c6c0','#d3aba2','#c39085','#b27469','#a05a4e','#8e4134','#7a261c','#660000'];

        if (type === 0) {
            return d3.rgb(red[Math.round(value * (10-1))]);
        } else if (type === 1) {
            return d3.rgb(blue[Math.round(value * (10-1))]);
        }
    }

    function _typeToString(type) {
        return type === 0 ? "Switch" : "Server";
    }

    function _setNodeUtilization(nodeId, util, type) {
        d3.select("#vdce_node_" + nodeId).attr("r", 4).style("fill", _nodeColor(type, util));
    }

    function _setLinkUtilization(linkId, util) {
        d3.select("#vdce_link_" + linkId).attr("stroke", _linkColor(util));
    }

    function _mouseOverLink() {
        var id = parseInt(d3.select(this).attr("id").split("_")[2]);
        $(".info > .id").html(id);
        $(".info > .type").html("Link");
        $(".info > .util").html((_network.links[id].utilization * 100) + "%");
    }

    function _mouseOverNode() {
        var id = parseInt(d3.select(this).attr("id").split("_")[2]);
        $(".info > .id").html(id);
        $(".info > .type").html(_network.nodes[id].type === 0 ? "Switch" : "Server");
        $(".info > .util").html(
            (_network.nodes[id].utilization[0] * 100) + "% / " +
            (_network.nodes[id].utilization[1] * 100) + "%"
        );
    }

    function _mouseOutLink() {
        $(".info > .id").html("");
        $(".info > .type").html("");
        $(".info > .util").html("");
    }

    function _mouseOutNode() {
        $(".info > .id").html("");
        $(".info > .type").html("");
        $(".info > .util").html("");
    }

    function _clickLink() {
        var id = parseInt(d3.select(this).attr("id").split("_")[2]);
        $(".info > .id").html(id);
        $(".info > .type").html("Link");
        $(".info > .util").html((_network.links[id].utilization * 100) + "%");
    }

    function _clickNode() {
        var id = parseInt(d3.select(this).attr("id").split("_")[2]);
        $(".info > .id").html(id);
        $(".info > .type").html(_network.nodes[id].type === 0 ? "Switch" : "Server");
        $(".info > .util").html(
            (_network.nodes[id].utilization[0] * 100) + "% / " +
            (_network.nodes[id].utilization[1] * 100) + "%"
        );
    }

    return {
        getTopology:    getTopology,
        getUtilization: getUtilization,
        requestVN:      requestVN,
        startSim:       startSim,
        continueSim:    continueSim,
        stopSim:        stopSim,
        getNetworks:    getNetworks,
        deleteVN:       deleteVN,
        randomVN:       randomVN,
        remap:          remap
    };
}

$(function() {


    var ui = new VDCEUI("127.0.0.1", 9090);
    var req = 0;

    function _loadView(group) {

        console.log("_loadView(" + group + ")");

        switch (group) {
            case "main-status":
                var tpl = $("#main-status-template").html();
                $(".ui.tab[data-tab='main-status']").html(Handlebars.compile(tpl));
                break;
            case "main-simulation":
                tpl = $("#main-simulation-template").html();
                $(".ui.tab[data-tab='main-simulation']").html(Handlebars.compile(tpl));
                break;
            case "main-networks":
                ui.getNetworks();
                break;
            case "main-scaling":
                tpl = $("#main-scaling-template").html();
                $(".ui.tab[data-tab='main-scaling']").html(Handlebars.compile(tpl));
                break;
        }
    }

    function _initDynamicControls(group) {

        console.log("_initDynamicControls(" + group + ")");

        switch (group) {
            case "main-status":
                break;
            case "main-simulation":

                $(".vc input[name=r]").val(req);
                $(".voc input[name=r]").val(req);

                $(".vc .action.rand").click(function() {
                    var r = random_vc();
                    $(".vc input[name=r]").val(req++);
                    $(".vc input[name=n]").val(r.n);
                    $(".vc input[name=c]").val(r.c);
                    $(".vc input[name=b]").val(r.b);
                });

                $(".vc .action.new").click(function() {
                    var r = parseInt($(".vc input[name=r]").val()),
                        n = parseInt($(".vc input[name=n]").val()),
                        b = parseInt($(".vc input[name=b]").val());
                    ui.requestVN(vc(r, n, b, b));
                });

                $(".voc .action.rand").click(function() {
                    var r = random_voc();
                    $(".voc input[name=r]").val(req++);
                    $(".voc input[name=n]").val(r.n);
                    $(".voc input[name=s]").val(r.s);
                    $(".voc input[name=c]").val(r.c);
                    $(".voc input[name=b]").val(r.b);
                    $(".voc input[name=o]").val(r.o);
                });

                $(".voc .action.new").click(function() {
                    var r = parseInt($(".voc input[name=r]").val()),
                        n = parseInt($(".voc input[name=n]").val()),
                        s = parseInt($(".voc input[name=s]").val()),
                        b = parseInt($(".voc input[name=b]").val()),
                        o = parseInt($(".voc input[name=o]").val());

                    ui.requestVN(voc(r, n, s, b, b, o));
                });

                $(".t3 .action.rand").click(function() {
                    var r = random_tiered();
                    $(".t3 input[name=r]").val(req++);
                    $(".t3 input[name=alpha]").val(r.alpha);
                    $(".t3 input[name=beta]").val(r.beta);
                    $(".t3 input[name=gamma]").val(r.gamma);
                    $(".t3 input[name=c]").val(r.c);
                    $(".t3 input[name=b]").val(r.b);
                });

                $(".t3 .action.new").click(function() {
                    var r     = parseInt($(".t3 input[name=r]").val()),
                        alpha = parseInt($(".t3 input[name=alpha]").val()),
                        beta  = parseInt($(".t3 input[name=beta]").val()),
                        gamma = parseInt($(".t3 input[name=gamma]").val()),
                        c     = parseInt($(".t3 input[name=c]").val()),
                        b     = parseInt($(".t3 input[name=b]").val());

                    ui.requestVN(tiered(r, alpha, beta, gamma, c, b));
                });

                $(".sim .action.start").click(function() {

                    var n    = parseInt($(".sim input[name=n]").val()),
                        type = $(".sim input[name=type]").val(),
                        del  = $(".sim input[name=del]").is(':checked');

                    ui.startSim(n, type, getRequest, del);
                });

                $(".del .action.delete").click(function() {
                    var i = parseInt($(".del input[name=i]").val());
                    ui.deleteVN(i);
                });

                $(".del .action.rand").click(function() {
                    var i = ui.randomVN();
                    $(".del input[name=i]").val(i);
                });

                $('.dropdown').dropdown();

                $('.sim-menu .item').tab();

                break;

            case "main-networks":
                break;
            case "main-scaling":

                $(".remap .action.remap").click(function() {
                    ui.remap();
                });

                break;
        }
    }

    function _uniform(a, b) {
        return Math.floor(Math.random() * (b - a)) + a;
    }

    function vc(r, n, c, b) {

        // r - request identifier
        // n - # virtual machines
        // c - capacity at VMs
        // b - bandwidth at VMs

        var net = { id: r, type: 0, nodes: [], edges: [] };
        net.nodes.push({id: 0, type: 0, group: 0, demand: [n * b, n * b]});

        for(var i = 1; i <= n; i++) {
            net.nodes.push({id: i, type: 1, group: 1, demand: [c, b]});
            net.edges.push({id: i-1, from: 0, to: i, demand: b});
        }

        return net;
    }

    function vc_expand(r, n) {

        var net = { id: r, type: 0, nodes: [], edges: [] };

        for(var i = 1; i <= n; i++) {
            net.nodes.push({id: i, type: 1, group: 1, demand: [0, 0]});
            net.edges.push({id: i-1, from: 0, to: i, demand: 0});
        }

        return net;
    }

    function voc(r, n, s, c, b, o) {

        // r - request identifier
        // n - # virtual machines
        // s - group size
        // c - capacity at VMs
        // b - bandwidth at VMs
        // o - oversubscription

        var net = {id: r, type: 1, nodes: [], edges: [] };

        net.nodes.push({id: 0, type: 0, group: 0, demand: [(n * b / o), (n * b / o)]});

        for (var i = 1; i <= n / s + 1; i++) {
            net.nodes.push({id: i, type: 0, group: 1, demand: [s * b, s * b]});
            net.edges.push({id: i - 1, from: 0, to: i, demand: b * s / o});
        }

        for (i = 1; i <= n / s + 1; i++) {
            for (var j = 0; j < s; j++) {
                net.nodes.push({id: 1+j+n/s-s+i*s, type: 1, group: 2, demand: [c, b]});
                net.edges.push({id: 1+j+n/s-s+i*s - 1, from: i, to: 1 + j + n/s - s + i * s, demand: b});
            }
        }

        return net;
    }

    function tiered(r, alpha, beta, gamma, c, b) {

        // r     - request identifier
        // alpha - # virtual machines in first group
        // beta  - # virtual machines in second group
        // gamma - # virtual machines in third group
        // c     - capacity at VMs
        // b     - bandwidth at VMs

        var net = {id: r, type: 2, nodes: [], edges: [] };

        var m = 0;

        net.nodes.push({id: 0, type: 0, group: 0, demand: [alpha * b + beta * b, alpha * b + beta * b]});
        net.nodes.push({id: 1, type: 0, group: 0, demand: [beta * b + gamma * b, beta * b + gamma * b]});

        for (var i = 1; i <= alpha; i++) {
            net.nodes.push({id: i + 1, type: 1, demand: [c, b]});
            net.edges.push({id: m++, from: i+1, to: 0, demand: b});
        }

        for (i = 1; i <= beta; i++) {
            net.nodes.push({id: i + alpha + 1, type: 1, demand: [c, b * 2]});
            net.edges.push({id: m++, from: i + alpha + 1, to: 0, demand: b});
            net.edges.push({id: m++, from: i + alpha + 1, to: 1, demand: b});
        }

        for (i = 1; i <= gamma; i++) {
            net.nodes.push({id: i + alpha + beta + 1, type: 1, demand: [c, b]});
            net.edges.push({id: m++, from: i + alpha + beta + 1, to: 1, demand: b});
        }

        return net;
    }

    function random_vc() {
        return {
            n: _uniform(10, 60),
            c: _uniform(50, 200),
            b: _uniform(50, 200)
        };
    }

    function random_voc() {
        var s = _uniform(4, 8);
        var m = _uniform(3, 6);

        return {
            n: s * m,
            s: s,
            c: _uniform(50, 200),
            b: _uniform(50, 200),
            o: _uniform(1, 4)
        };
    }

    function random_tiered() {
        return {
            alpha: _uniform(2, 16),
            beta: _uniform(2, 16),
            gamma: _uniform(2, 16),
            c: _uniform(50, 200),
            b: _uniform(50, 200)
        };
    }

    function getRequest(type) {

        req++;

        if (type === "vc") {
            var r = random_vc();
            var net = vc(req, r.n, r.c, r.b);
            return net;
        } else if (type === "voc") {
            var r = random_voc();
            var net = voc(req, r.n, r.s, r.c, r.b, r.o);
            return net;
        } else if (type === "3t") {
            var r = random_tiered();
            var net = tiered(req, r.alpha, r.beta, r.gamma, r.c, r.b);
            return net;
        } else if (type === "mix") {
            var c = _uniform(0, 3);
            switch(c) {
                case 0:
                    var r = random_vc();
                    var net = vc(req, r.n, r.c, r.b);
                    return net;
                    break;
                case 1:
                    var r = random_voc();
                    var net = voc(req, r.n, r.s, r.c, r.b, r.o);
                    return net;
                    break;
                case 2:
                    var r = random_tiered();
                    var net = tiered(req, r.alpha, r.beta, r.gamma, r.c, r.b);
                    return net;
                    break;
            }
        }
    }

    _loadView("main-status");
    _initDynamicControls("main-status");

    $('.main-menu .item').tab({
        onFirstLoad: function(tab) {
            _loadView(tab);
            _initDynamicControls(tab);
        },
        onVisible: function(tab) {
            // console.log("onVisible: " + tab);
            switch(tab) {
                case "main-status":
                    break;
                case "main-simulation":
                    break;
                case "main-networks":
                    _loadView("main-networks");
                    _initDynamicControls("main-networks");
                    break;
                case "main-scaling":
                    break;
            }
        }
    });
});
