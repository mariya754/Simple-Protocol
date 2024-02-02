#!/bin/bash

docker run --privileged --rm -ti -v .:/opt cmsc389z

ip netns add ep1
ip netns add ep2
ip netns add ep3
ip netns add router
ip link add devA type veth peer devR1
ip link add devB type veth peer devR2
ip link add devC type veth peer devR3

ip link set devA netns ep1
ip link set devB netns ep2
ip link set devC netns ep3

ip link set devR1 netns router
ip link set devR2 netns router
ip link set devR3 netns router

ip netns exec ep1 ip link set lo up
ip netns exec ep2 ip link set lo up
ip netns exec ep3 ip link set lo up
ip netns exec router ip link set lo up

ip netns exec ep1 ip addr add 186.192.0.11/32 dev devA 
ip netns exec ep2 ip addr add 186.192.0.12/32 dev devB
ip netns exec ep3 ip addr add 186.192.0.13/32 dev devC
ip netns exec router ip addr add 186.192.0.1/32 dev devR1
ip netns exec router ip addr add 186.192.0.1/32 dev devR2
ip netns exec router ip addr add 186.192.0.1/32 dev devR3

ip netns exec ep1 ip link set devA up
ip netns exec ep2 ip link set devB up 
ip netns exec ep3 ip link set devC up
ip netns exec router ip link set devR1 up 
ip netns exec router ip link set devR2 up
ip netns exec router ip link set devR3 up 


ip netns exec ep1 ip route add 186.192.0.0/24 dev devA \
proto static scope global src 186.192.0.11
ip netns exec ep2 ip route add 186.192.0.0/24 dev devB \
proto static scope global src 186.192.0.12
ip netns exec ep3 ip route add 186.192.0.0/24 dev devC \
proto static scope global src 186.192.0.13
ip netns exec router ip route add 186.192.0.0/24 dev devR1 \
proto static scope global src 186.192.0.1

ip netns exec router ip link add name bridgeDev type bridge
ip netns exec router ip address add dev bridgeDev local 186.192.0.1
ip netns exec router ip link set dev bridgeDev up
ip netns exec router ip link set dev devR1 master bridgeDev
ip netns exec router ip link set dev devR2 master bridgeDev
ip netns exec router ip link set dev devR3 master bridgeDev

ip netns exec ep1 tc qdisc add dev devA root handle 1:0 netem delay 10ms loss random 30
ip netns exec ep2 tc qdisc add dev devB root handle 1:0 netem delay 10ms loss random 10
ip netns exec ep3 tc qdisc add dev devC root handle 1:0 netem delay 100ms loss random 10

ip netns exec router tc qdisc add dev devR1 root handle 1:0 netem delay 10ms loss random 30
ip netns exec router tc qdisc add dev devR2 root handle 1:0 netem delay 10ms loss random 10
ip netns exec router tc qdisc add dev devR3 root handle 1:0 netem delay 100ms loss random 10

chmod +x start_testbed.sh

