main: main.cpp Action.cpp Item.cpp Transaction.h Action.h Item.h Transaction.h
	g++ main.cpp Action.cpp Item.cpp Transaction.cpp -std=c++2a -o main