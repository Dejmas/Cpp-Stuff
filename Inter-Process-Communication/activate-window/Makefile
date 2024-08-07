# Compile WindowActivator.mm
all: activate

main.o: main.cpp
	clang++ -c $< -o $@

WindowActivator.o: WindowActivator.mm
	clang++ -c $< -o $@ -framework Cocoa

# Link the object files together into the final executable
activate: main.o WindowActivator.o
	clang++ $^ -o $@ -framework Cocoa

clean:
	rm -rf main.o activate WindowActivator.o