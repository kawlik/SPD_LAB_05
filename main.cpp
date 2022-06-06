#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

/*  Declarations
/*   *   *   *   *   *   *   *   *   *   */

typedef struct task {
    int r, p, q, x;
} task_t;

typedef struct block {
    int a, b, c;
} block_t;

void readTill( string fileHeader, ifstream &data, vector<task_t> &tasks );

block makeBlock( vector<task_t> &tasks );
int schrage_std( vector<task_t> &tasks );
int schrage_alt( vector<task_t> &tasks );
int carlier( vector<task_t> &tasks, int cmax );


/*  Executor
/*   *   *   *   *   *   *   *   *   *   */

int main( int argc, char **argv ) {

    //  get method STD or ALT
    bool set_OUT = (( string( argv[1] ) == "out" ) ? true : false );

    //  initial dataset
    vector<task_t> tasks;
    ifstream data( "carl.data.txt" );
    int totalTime = 0;

    //  process for all 4 datasets
    for ( int i = 0; i <= 8 ; i++ ) {

        //  reset tasks
        tasks.clear();

        //  create new file header
        string fileHeader = "data.00" + to_string( i ) + ":";

        //  read data
        readTill( fileHeader, data, tasks );

        //  get partial result
        int schrag = schrage_std( tasks );
        int result = carlier( tasks, schrag );

        //  print result
        cout << endl;
        cout << "---+---+---+---+---+---+---+---+---+---" << endl;

        cout << "Current run: " << i << endl;
        cout << "Calculated time: " << result << endl;


        //  print order
        if( set_OUT ) {

            cout << "Order:" << endl;

            for( int i = 0; i < tasks.size(); i++ ) {
                cout << setw( 2 ) << i + 1 << ". [ r: " << setw( 6 ) << tasks[i].r << " | p: " << setw( 6 ) << tasks[i].p << " | q: " << setw( 6 ) << tasks[i].q << " | x: " << setw( 2 ) << tasks[i].x << "]" << endl;
            }
        }

        cout << "---+---+---+---+---+---+---+---+---+---" << endl;
        cout << endl;

        //  update total result
        totalTime += result;
    }

    //  print result
    cout << endl;
    cout << "---+---+---+---+---+---+---+---+---+---" << endl;

    cout << "<*> Final result <*>" << endl;
    cout << "Calculated total time: " << totalTime << endl;

    cout << "---+---+---+---+---+---+---+---+---+---" << endl;
    cout << endl;

    //  close data stream
    data.close();

    //  final statement
    return 0;
}


/*  Solutions
/*   *   *   *   *   *   *   *   *   *   */

void readTill( string fileHeader, ifstream &data, vector<task_t> &tasks ) {

    //  initial dataset
    string buffer;
    int size;

    //  read till heder found
    do {
        data >> buffer;
    } while( buffer != fileHeader );

    //  read size
    data >> size;

    //  read data
    for( int  i = 0; i < size; i++ ) {

        //  task buffer
        task_t tmp;

        //  read data into task
        data >> tmp.r >> tmp.p >> tmp.q;

        //  set task id
        tmp.x = i;

        //  add task to list
        tasks.push_back( tmp );
    }
}

block makeBlock( vector<task_t> &tasks ) {

    //  initial dataset
    block result = { -1, -1, -1 };

    int c = 0;
    int C = 0;
    int m = 0;

    int t = tasks[0].r;

    //  initial eval
    for( int i = 0; i < tasks.size(); i++ ) {
        m = max( m, tasks[i].r ) + tasks[i].p;
        C = max( C, m + tasks[i].q );
    }

    /*  algorithm
    /*   *   *   *   *   *   *   *   */

    for(int i = 0; i < tasks.size(); i++) {

        //  update time count
        t = max(t, tasks[i].r);
        t += tasks[i].p;

        //  test overflow
        if((t + tasks[i].q) == C) { result.b = i; }

        //  update c value
        c = max(c, t + tasks[i].q);
    }

    //  peprepare for next stage
    t = tasks[0].r;
    c = 0;

    for(int i = 0; i < tasks.size(); i++) {

        //  update time count
        t = max(t, tasks[i].r);
        t += tasks[i].p;

        //  test overflow
        if(result.a == -1) {

            //  init accumulator
            int s = 0;

            //  update accumulator
            for(int j = 0; j <= result.b; j++) {
                s += tasks[j].p;

                if(j == result.b) { s += tasks[j].q; }    
            }

            //  update result
            if(C == tasks[i].r + s) { result.a = i; }
        }

        //  update c value
        c = max(c, t + tasks[i].q);
    }
    
    //  peprepare for next stage
    t = tasks[0].r;
    c = 0;

    for(int i = 0; i < tasks.size(); i++) {

        if(tasks[i].q < tasks[result.b].q) { c = i; }
    }

    //  return result
    return result;
}

int schrage_std( vector<task_t> &tasks ) {

    //  initial dataset
    int m = 0;
    int c = 0;


    /*  scheduling algorithm
    /*   *   *   *   *   *   *   *   */

    //  initial values
    int t = 0;
    vector<task_t> N = tasks;
    vector<task_t> G;
    vector<task_t> K;

    //  initial N set stable_sort
    stable_sort( N.begin(), N.end(), []( task_t const &p, task_t const &q ) -> bool {
        return p.r < q.r;
    });

    //  starts procedure
    while( G.size() || N.size() ) {
        
        //  first step
        while( N.size() && N.front().r <= t ) {

            //  get task
            task_t tmp = N.front();

            //  erase task from N set
            N.erase( N.begin() );

            //  add task to G set
            G.push_back( tmp );
        }

        //  second step
        if( !G.size() ) {

            //  update t value
            t = N.front().r;

        } else {

            //  restable_sort G set
            stable_sort( G.begin(), G.end(), []( task_t const &p, task_t const &q ) -> bool {
                return p.q > q.q;
            });

            //  get task
            task_t tmp = G.front();

            //  erase task from G set
            G.erase( G.begin() );

            //  add task to K set
            K.push_back( tmp );

            //  update t value
            t += tmp.p;
        }
    }

    //  update taksk ref
    tasks = K;

    /*   *   *   *   *   *   *   *   */


    //  recalc values
    for( int i = 0; i < tasks.size(); i++ ) {
        m = max( m, tasks[i].r ) + tasks[i].p;
        c = max( c, m + tasks[i].q );
    }

    //  return result
    return c;
}

int schrage_alt( vector<task_t> &tasks ) {

    //  initial dataset
    int m = 0;
    int c = 0;


    /*  scheduling algorithm
    /*   *   *   *   *   *   *   *   */

    //  initial values
    int t = 0;
    vector<task_t> N = tasks;
    vector<task_t> G;
    vector<task_t> K;


    //  initial N set stable_sort
    stable_sort( N.begin(), N.end(), []( task_t const &p, task_t const &q ) -> bool {
        return p.r < q.r;
    });

    //  starts procedure
    while( G.size() || N.size() ) {
        
        //  first step
        while( N.size() && N.front().r <= t ) {

            //  get task
            task_t tmp = N.front();

            //  erase task from N set
            N.erase( N.begin() );

            //  add task to G set
            G.push_back( tmp );

            //  test devide
            if( tmp.q > G.front().p ) {

                //  postpone task
                G.front().p = t - tmp.r;

                //  update time
                t = tmp.r;
            }
        }

        //  second step
        if( !G.size() ) {

            //  update t value
            t = N.front().r;

        } else {

            //  restable_sort G set
            stable_sort( G.begin(), G.end(), []( task_t const &p, task_t const &q ) -> bool {
                return p.q > q.q;
            });

            //  get task
            task_t tmp = G.front();

            //  erase task from G set
            G.erase( G.begin() );

            //  add task to K set
            K.push_back( tmp );

            //  update t value
            t += tmp.p;
        }
    }

    //  update taksk ref
    tasks = K;

    /*   *   *   *   *   *   *   *   */


    //  recalc values
    for( int i = 0; i < tasks.size(); i++ ) {
        m = max( m, tasks[i].r ) + tasks[i].p;
        c = max( c, m + tasks[i].q );
    }

    //  return result
    return c;
}

int carlier( vector<task_t> &tasks, int cmax ) {

    //  initial dataset
    int m = 0;
    int c = 0;

    //  calc block
    block B = makeBlock(tasks);


    /*  carlier initial stage
    /*   *   *   *   *   *   *   *   */

    //  test integrity
    if(c == -1) return 999999;

    int pi = 999999;
    int qi = 999999;
    int ri = 999999;


    /*  scheduling algorithm
    /*   *   *   *   *   *   *   *   */

    for(int i = B.c + 1; i < B.b; i++) {

        //  tests
        if(tasks[i].r < ri) { ri = tasks[i].r ; }
        if(tasks[i].q < qi) { ri = tasks[i].q ; }

        //  update p
        pi += tasks[i].p;
    }


    //  r test
    int r = tasks[B.c].r;
    tasks[B.c].r = max(tasks[B.c].r, ri + pi);

    int lr = schrage_alt(tasks);

    //  test calculated value
    if(lr < cmax) { carlier(tasks, lr); }

    //  revert
    tasks[B.c].r = r;


    //  q test
    int q = tasks[B.c].q;
    tasks[B.c].q = max(tasks[B.c].q, qi + pi);

    int lq = schrage_alt(tasks);

    //  test calculated value
    if(lq < cmax) { carlier(tasks, lq); }

    //  revert
    tasks[B.c].q = q;


    /*  final stage
    /*   *   *   *   *   *   *   *   */

    //  recalc values
    for( int i = 0; i < tasks.size(); i++ ) {
        m = max( m, tasks[i].r ) + tasks[i].p;
        c = max( c, m + tasks[i].q );
    }

    //  return result
    return c;
}