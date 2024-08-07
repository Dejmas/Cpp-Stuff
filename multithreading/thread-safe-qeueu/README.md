# Thread Save Queue

Here is a simple implementation for threaded queue. In this implmementation the queue is implemented by circular buffer. Method `pushItem` waits when queue is full until there is a capacity. Method `popItem` waits when queue is empty until there is something to be poped.