#!/usr/bin/env ruby

require 'httparty'

class CloudSim
  include HTTParty
  base_uri 'http://localhost:9090'


  def _vc(r, n, b, c)
    net = { id: r, type: 0, nodes: [], edges: [] }
    net[:nodes] << {id: 0, type: 0, group: 0, demand: [n * b, n *b]}

    for i in 1..n
      net[:nodes] << {id: i, type: 1, group: 1, demand: [c, b]}
      net[:edges] << {id: i-1, from: 0, to: i, demand: b}
    end

    net.to_json()
  end

  def _voc(r, n, s, c, b, o)
    net = { id: r, type: 0, nodes: [], edges: [] }
    net[:nodes] << {id: 0, type: 0, group: 0, demand: [(n * b / o), (n * b / o)]}

    for i in 1..(n/s+1)
      net[:nodes] << {id: i, type: 0, group: 1, demand: [s * b, s * b]}
      net[:edges] << {id: i - 1, from: 0, to: i, demand: b * s / o}
    end

    for i in 1..(n/s+1)
      for j in 0..s-1
        net[:nodes] << {id: 1+j+n/s-s+i*s, type: 1, group: 2, demand: [c, b]}
        net[:edges] << {id: 1+j+n/s-s+i*s - 1, from: i, to: 1 + j + n/s - s + i * s, demand: b}
      end
    end

    net.to_json()
  end

  def new_vc(r, n, b, c)
    body = _vc(r, n, b, c)
    self.class.post("/vnets", body: body)
  end

  def new_voc(r, s, m, c, b, o)
    body = _voc(r, s * m, s, c, b, o)
    self.class.post("/vnets", body: body)
  end

  def vnets
    response = self.class.get("/vnets")
    hash = JSON.parse(response.body)
    return hash['networks']
  end

  def expand_vn(id)
    self.class.put("/vnets/#{id}")
  end

  def delete_vn(id)
    self.class.delete("/vnets/#{id}")
  end

end

cs = CloudSim.new()

new_r = 0

80.times do |i|
  r = Random.new()
  new_r += 1
  opt = r.rand(1..3)
  case opt
  when 1
    puts "new vc #{new_r}"
    cs.new_vc(new_r, rand(10..60), rand(50..200), rand(50..200))
  when 2
    puts "new voc #{new_r}"
    cs.new_voc(new_r, rand(4..8), rand(3..6), rand(50..200), rand(50..200), rand(1..4))
  when 3
    puts "new voc #{new_r}"
    cs.new_voc(new_r, rand(4..8), rand(3..6), rand(50..200), rand(50..200), rand(1..4))
  end
end

1000.times do |i|
  r = Random.new()
  opt = r.rand(1..5)

  case opt
  when 1
    vnets = cs.vnets()
    unless vnets.empty?
      id = vnets.sample()['id']
      puts "expand #{id}"
      cs.expand_vn(id)
    end
  when 2
    vnets = cs.vnets()
    unless vnets.empty?
      id = vnets.sample()['id']
      puts "expand #{id}"
      cs.expand_vn(id)
    end
  when 3
    vnets = cs.vnets()
    unless vnets.empty?
      id = vnets.sample()['id']
      puts "delete #{id}"
      cs.delete_vn(id)
    end
  when 4
    new_r += 1
    opt = rand(1..3)
    case opt
    when 1
      puts "new vc #{new_r}"
      cs.new_vc(new_r, rand(10..60), rand(50..200), rand(50..200))
    when 2
      puts "new voc #{new_r}"
      cs.new_voc(new_r, rand(4..8), rand(3..6), rand(50..200), rand(50..200), rand(1..4))
    when 3
      puts "new voc #{new_r}"
      cs.new_voc(new_r, rand(4..8), rand(3..6), rand(50..200), rand(50..200), rand(1..4))
    end

  when 5
    new_r += 1
    opt = rand(1..3)
    case opt
    when 1
      puts "new vc #{new_r}"
      cs.new_vc(new_r, rand(10..60), rand(50..200), rand(50..200))
    when 2
      puts "new voc #{new_r}"
      cs.new_voc(new_r, rand(4..8), rand(3..6), rand(50..200), rand(50..200), rand(1..4))
    when 3
      puts "new voc #{new_r}"
      cs.new_voc(new_r, rand(4..8), rand(3..6), rand(50..200), rand(50..200), rand(1..4))
    end
  end
end
