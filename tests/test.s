#ORG &1000
.entry LDX #0
.loop  LDA data,X
JSR &FFE3
INX
#ORG &900
CPX #&20
BNE loop
RMB ~3,temp
RTS
.data
#EQUB 20
#EQUS "This is a text string"
#EQUD 12345678
#CONST data &1000
.lastone
