In this project, I implemented MyMaze.java and also Coordinates.java. Everything else was given to me as helper files.
This project uses stack and queue data structures for the overall functionality. A maze is created
by choosing a random direction to go to and checking if it has been visited or not. If it has not been visited then
it is added to the stack and the wall gets removed between the neighboring cell and the current cell. This process
is repeated until the stack is empty.

The print maze function prints out the maze that was genereated in the makeMaze method.

The solveMaze method uses a queue structure to set each corresponding cell's visited attribute to true.
It also enqueue's all reachable neighbors that are un-visited and marks them with a "*". If the current cell
is currently at the finish point of rows-1,columns-1 then the maze has been solved.