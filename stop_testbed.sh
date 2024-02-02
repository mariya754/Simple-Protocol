#!/bin/bash

ip netns delete ep1
ip netns delete ep2
ip netns delete ep3
ip netns delete router

chmod +x start_testbed.sh