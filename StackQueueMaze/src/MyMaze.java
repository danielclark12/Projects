// Names:
// x500s:

import java.util.Random;

public class MyMaze{
    Cell[][] maze;

    public MyMaze(int rows, int cols) {
        maze = new Cell[rows][cols];
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                maze[i][j] = new Cell();
            }
        }
    }

    /* TODO: Create a new maze using the algorithm found in the writeup. */
    public static MyMaze makeMaze(int rows, int cols) {

        MyMaze newMaze = new MyMaze(rows,cols);
        Stack1Gen<Coordinates> myStack = new Stack1Gen<Coordinates>();
        Coordinates start = new Coordinates(0,0); // Initialize helper class to keep track of coordinates
        Coordinates next = new Coordinates(0,0);
      //  Coordinates New = new Coordinates(0,0);

        myStack.push(start);

        newMaze.maze[0][0].setVisited(true);
        int direction = 0;
        Coordinates top = new Coordinates();

        while(!myStack.isEmpty()){
            top = myStack.top();
            int currentXIndex = top.getX();
            int currentYIndex = top.getY();

            int available = 0;

            // Check neighbors for availability
            // check right
            if (currentYIndex + 1 < newMaze.maze[0].length && !newMaze.maze[currentXIndex][currentYIndex + 1].getVisited()){
                available++; }

            // check left
            if(currentYIndex - 1 >= 0 && !newMaze.maze[currentXIndex][currentYIndex - 1].getVisited()){
                available++;}

            // check down
            if(currentXIndex + 1 < newMaze.maze.length && !newMaze.maze[currentXIndex + 1][currentYIndex].getVisited()){
                available++;}

            // check up
            if(currentXIndex - 1 >= 0 && !newMaze.maze[currentXIndex-1][currentYIndex].getVisited()){
                available++;}

            // if available = 0 we know to pop otherwise pick a direction to move
            if(available > 0) {
                // pick random direction
                direction = new Random().nextInt(4) + 1;

                if (direction == 1 ) { // go right
                    if (currentYIndex + 1 < newMaze.maze[0].length && !newMaze.maze[currentXIndex][currentYIndex + 1].getVisited()) {
                        next.setY(currentYIndex + 1);
                        newMaze.maze[currentXIndex][currentYIndex + 1].setVisited(true);
                        newMaze.maze[currentXIndex][currentYIndex].setRight(false);
                        myStack.push(new Coordinates(next.getX(),next.getY()));
                    }
                } else if (direction == 2 ) { // go left
                    if (currentYIndex - 1 >= 0 && !newMaze.maze[currentXIndex][currentYIndex - 1].getVisited()) {
                        next.setY(currentYIndex - 1);
                        newMaze.maze[currentXIndex][currentYIndex - 1].setVisited(true);
                        newMaze.maze[currentXIndex][currentYIndex - 1].setRight(false);
                        myStack.push(new Coordinates(next.getX(),next.getY()));
                    }
                } else if (direction == 3 ) { // go down
                    if (currentXIndex + 1 < newMaze.maze.length && !newMaze.maze[currentXIndex + 1][currentYIndex].getVisited()) {
                        next.setX(currentXIndex+1);
                        newMaze.maze[currentXIndex+1][currentYIndex].setVisited(true);
                        newMaze.maze[currentXIndex][currentYIndex].setBottom(false);
                        myStack.push(new Coordinates(next.getX(),next.getY()));
                    }
                } else if (direction == 4) { // go up
                    if (currentXIndex - 1 >= 0 && !newMaze.maze[currentXIndex-1][currentYIndex].getVisited()) {
                        next.setX(currentXIndex-1);
                        newMaze.maze[currentXIndex-1][currentYIndex].setVisited(true);
                        newMaze.maze[currentXIndex-1][currentYIndex].setBottom(false);
                        myStack.push(new Coordinates(next.getX(),next.getY()));
                    }
                }
            }

            // If no neighbors then pop
            else{
                if(!myStack.isEmpty())
                    myStack.pop();
            }
        }

        // reset all cells visited attribute to false
        for(int i = 0; i < newMaze.maze.length; i++){
            for(int j = 0; j < newMaze.maze[0].length; j++){
                newMaze.maze[i][j].setVisited(false);
            }
        }

        return newMaze;
    }
    /* TODO: Print a representation of the maze to the terminal */
    public void printMaze(boolean path) {

        System.out.print("|");
        // print top border
        for(int i = 0; i < maze[0].length; i++){
            System.out.print("---|");
        }
        System.out.print("\n");

        // start into the maze
        for(int i = 0; i < 2*maze.length; i++){
            for(int j = 0; j < maze[0].length; j++) {
                if(j == 0 && i == 0)
                    System.out.print(" "); // print a space at entrance of maze
                else if(j == 0)
                    System.out.print( "\n|"); // print the left border

                if(i % 2 == 0){ // Even rows will contain asterisks
                    if(path){
                        if(maze[i/2][j].getVisited()){
                            System.out.print(" * ");
                        }
                        else System.out.print("   ");
                    }
                    else{
                        System.out.print("   ");
                    }

                    if(maze[i/2][j].getRight()){
                        if(j == maze[0].length - 1 && i == 2*maze.length - 2){
                            System.out.print(" ");
                        }
                        else
                        System.out.print("|");
                    }
                    else System.out.print(" ");
                }

                else { // odd rows

                    if((i/2) >= 0){
                        if(maze[(i-1)/2][j].getBottom()){
                            System.out.print("---|");
                     }
                        else System.out.print("   |");}
                }
            }
        }
    }

    /* TODO: Solve the maze using the algorithm found in the writeup. */
    public void solveMaze() {
        Q1Gen<Coordinates> myQ = new Q1Gen<Coordinates>();
        Coordinates start = new Coordinates(0,0);
        myQ.add(start);
        int counter = 0;
        while(!myQ.isEmpty()){
            counter++;
            // get coordinates from element
            Coordinates front = myQ.remove();
            int currentXIndex = front.getX();
            int currentYIndex = front.getY();
            maze[currentXIndex][currentYIndex].setVisited(true);

            // Check if maze has been solved:
            if(currentXIndex == maze.length - 1 && currentYIndex == maze[0].length - 1){
               // System.out.println("Maze has been solved! after " + counter);
                break;
            }
            else{
                // Check all possible neighbors around for unvisited and add to queue

                // Check left
                if(currentYIndex - 1 >= 0){
                    if(!maze[currentXIndex][currentYIndex - 1].getVisited() && !maze[currentXIndex][currentYIndex - 1].getRight()){myQ.add(new Coordinates(currentXIndex,currentYIndex-1));}
                }

                // Check Right
                if(currentYIndex + 1 < maze[0].length){
                    if(!maze[currentXIndex][currentYIndex + 1].getVisited() && !maze[currentXIndex][currentYIndex].getRight()){myQ.add(new Coordinates(currentXIndex,currentYIndex+1));}
                }

                // Check Up
                if(currentXIndex - 1 >= 0){
                    if(!maze[currentXIndex - 1][currentYIndex].getVisited() && !maze[currentXIndex - 1][currentYIndex].getBottom()){myQ.add(new Coordinates(currentXIndex-1,currentYIndex));}
                }

                // Check down
                if(currentXIndex + 1 < maze.length){
                    if(!maze[currentXIndex + 1][currentYIndex].getVisited() && !maze[currentXIndex][currentYIndex].getBottom()){myQ.add(new Coordinates(currentXIndex+1,currentYIndex));}
                }
            }


        }
       printMaze(true);
    }

    public static void main(String[] args){
        /* Any testing can be put in this main function */
        MyMaze test = new MyMaze(5,20);
        test = makeMaze(5,20);
        //test.printMaze(false);


        test.solveMaze();
        //test.printMaze(false);
        System.out.println();
        System.out.println();
        //test.printMaze(true);

    }
}
