set opt(chan)           Channel/WirelessChannel;	# channel layer
set opt(prop)           Propagation/TwoRayGround;	# radio-propagation model
set opt(netif)          Phy/WirelessPhy;		# physical layer
set opt(ant)            Antenna/OmniAntenna;		# antenna model
set opt(macLayer)       Mac;				# mac layer
set opt(ifq)            Queue/DropTail;			# message queue, ifq
set opt(lenIfq)         50;				# max packet in ifq
set opt(linkLayer)      LL;				# link layer
set opt(routing)        DSDV;			        # routing protocol
set opt(agent)		Agent/MyAgent;		# simulation agent

Agent/MyAgent set myVal_ 0

# topology
set opt(numOfNodes)   	41;				# number of mobilenodes
set opt(width)		400;				# topography width
set opt(height)		400;				# topography height

set ns_	        [new Simulator]
set chan_	[new $opt(chan)]
set topo_	[new Topography]

#set basic packet tracing
set tracefd     [open "trace.tr" w]
$ns_ trace-all 	$tracefd

#create god object
set god_	[create-god $opt(numOfNodes)]

# create topology object
$topo_ load_flatgrid $opt(width) $opt(height)

# general node configuration
$ns_ node-config \
	-adhocRouting $opt(routing) \
	-llType $opt(linkLayer) \
	-macType $opt(macLayer) \
	-ifqType $opt(ifq) \
	-ifqLen $opt(lenIfq) \
	-antType $opt(ant) \
	-propType $opt(prop) \
	-phyType $opt(netif) \
	-topoInstance $topo_ \
	-channel $chan_ \
	-agentTrace OFF \
	-routerTrace OFF \
	-macTrace OFF \
	-movementTrace OFF \
	-energyModel "EnergyModel" \
	-rxPower 1.0 \
	-txPower 1.0 \
	-initialEnergy 1000 \
	-sleepPower .5 \
	-transitionPower .2 \
	-transitionTime .001 \
	-idlePower .1

for {set i 0} {$i < $opt(numOfNodes)} {incr i} {
	# init mobile nodes
	set node_($i) [$ns_ node]

	# disable random motion
	$node_($i) random-motion 0

	# register node.
	$god_ new_node $node_($i)

	# initial node position
	$ns_ initial_node_pos $node_($i) 10

	# init agent
	set agent_($i) [new $opt(agent)]
		
	# attach agent to node.
	$node_($i) attach $agent_($i)
} 

###############################################
#### Added by Parul #######
$ns_ node-config -initialEnergy 1300
set node_(0) [$ns_ node]

# set initial energy for second node
$ns_ node-config -initialEnergy 1500
set node_(1) [$ns_ node]

#Error when setting initial energy for third node.
#Figure out reasoning, Take Note that node_(4) 
#gives same error but at different time

# set initial energy for fourth node
$ns_ node-config -initialEnergy 1600
set node_(3) [$ns_ node]
###############################################

set sizeRNG [new RNG]
for {set j 0} {$j < $opt(numOfNodes) } {incr j} {
	set randX [$sizeRNG uniform 0 $opt(width)];
	set randY [$sizeRNG uniform 0 $opt(height)];
	$node_($j) set X_ $randX
	$node_($j) set Y_ $randY
	$node_($j) set Z_ 0.000000000000
}

# schedule
$ns_ at 20 "$agent_(0) sendTo -1"
$ns_ at 21 "$agent_(0) sendTo -1"
$ns_ at 22 "$agent_(0) sendTo -1"
$ns_ at 40 "$ns_ halt"

# start simulation
$ns_ run;

$ns_ flush-trace
close $tracefd
