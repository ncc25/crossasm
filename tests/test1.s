LDX &1234,X
LDA end,X
LDA (&71,X)
.alabel
.label2 ORA #&85 /a comment
CMP uklabel,X
BBR ~2,&6F,end
BRK / another comment
BEQ alabel
CLC
.end ADC (alabel),Y

