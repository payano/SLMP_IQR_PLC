# SLMP_IQR_PLC

DEMO CODE

If there is a delay for a couple of minutes before the last SLMP message was sent the PLC can hickup and stop respond to TCP messages. A fix for that solution is to ping the PLC 5 times and check if the port is open before issuing the SLMP command. The bash command for that is:

ping -c5 10.46.205.53 && sleep 1 && nc -zv 10.46.205.53 5007 && sleep 1 && ./SLMP 
