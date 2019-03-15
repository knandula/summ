JAVAC = javac
CCOPTS = -O

JAVA_OBJS = GGPerf.class \
            GGNode.class \
            GGRandom.class \
				GGSymExp.class \
				GGTable.class
				 
JAVA_SRC  = GGPerf.java \
            GGNode.java \
            GGRandom.java \
			   GGSymExp.java \
			   GGTable.java

all: $(JAVA_OBJS)

%.class: %.java
	$(JAVAC) $(CCOPTS) $(JAVA_SRC)

.PHONY: clean

clean:
	rm -f *.class *~ core *.exe.stackdump
