#FONT=aterm14tab.bmp
FONT=firefox.bmp
#FONT=notepad13tab.bmp
#FONT=msdos12tab.bmp
#FONT=sap.bmp
#GAMMA=1.8
GAMMA=0.7
all:	asciiart
asciiart:
	gcc -DFONTFILE=\"$(FONT)\" -DPROIBIDOS=\"\\x35\\x3e\" -DHTML_OUT=0 -DDEBUG -DGAMMA=$(GAMMA) asciiart5.c -o asciiart -lm
