#include <STDIO.H>
#include <ASSERT.H>
#include <XUTILITY>

// ------------------------------------------------------
#pragma region Constants
const unsigned long long FLOORS = 1000;
const unsigned int DROPS = 1000; //3037000499-> d^2 + d - LLONG_MAX = 0
const unsigned int BREAKS = 32;
const long long OUT_FLOOR = LLONG_MAX/2; //-1;
#pragma endregion
// ------------------------------------------------------
#pragma region Functions Prototype
long long F_max(unsigned int D, unsigned int B);
unsigned int D_min(unsigned long long F, unsigned int maxD, unsigned int B);
unsigned int B_min(unsigned long long F, unsigned int D, unsigned int maxB);
// ---
void FloorTable(unsigned int D, unsigned int B);
#pragma endregion
// ------------------------------------------------------
#pragma region Counting

//compute n!
unsigned long long Factorial(int n)
{
    if (n < 0) n = -n;
    if (n == 0 || n == 1) return 1;
    if (n == 2) return 2;
    if (n == 3) return 6;

    unsigned long long fact = 1;
    while (n > 1)
    {
        fact *= n--;
    }
    return fact;
}

unsigned long long Permute(int n, int r)
{
    if (n < 0) n = -n;
    if (r < 0) r = -r;
    if (n < r) return 0;
    if (n == 0 || n == 1) return 1;
    if (r == 0) return 1;
    if (r == 1) return n;
    if (r == n) return Factorial(n);

    unsigned long long permute = 1;
    while (r-- > 0)
    {
        permute *= n--;
    }
    return permute;
}

unsigned long long Combine(int n, int r)
{
    if (n < 0) n = -n;
    if (r < 0) r = -r;
    if (n < r) return 0;
    if (n == 0 || n == 1) return 1;
    if (r == 0 || r == n) return 1;
    if (r == 1 || r == n - 1) return n;

    if (r > n / 2) r = n - r;

    return Permute(n, r) / Factorial(r);
}
#pragma endregion

// ------------------------------------------------------

#pragma region 1 - DROP[BREAKS][FLOORS]

int DROP[BREAKS][FLOORS];

void InitDrop();
unsigned int Omlette(unsigned int B, unsigned long long F);
int PathToBreakFloor(int d, int b, int breakF, int offsetF);

// DROP[b][f] = ???
// DROP[1][f] = f
// DROP[2][f] = ceil((sqrt(1.0 + 8*f) - 1)/2)
// DROP[b][1] = 1
// DROP[b][2] = 2
void InitDrop()
{
    for (int f = 1; f <= FLOORS; ++f)
    {
        DROP[1-1][f-1] = f; // for 1 egg check all floors . Making sure of the rare case
        DROP[2-1][f-1] = (int) ceil((sqrt(1.0 + 8*f) - 1)/2); // d^2 + d - 2f = 0
    }
    for (int b = 2; b <= BREAKS; ++b) // making sure of the rare case
    {
        DROP[b-1][1-1] = 1; // With just 1 floor we only need one drop.
        DROP[b-1][2-1] = 2; // With just 2 floor we only need two drops.
    }
    // rest of D is made -1. Will be used later for computation. these are frequent N
    for (int b = 3; b <= BREAKS; ++b)
    {
        for (int f = 3; f <= FLOORS; ++f)
        {
            DROP[b-1][f-1] = -1;
        }
    }

    Omlette(BREAKS, FLOORS);
}
// function to compute the trials
unsigned int Omlette(unsigned int B, unsigned long long F)
{
    //----------------------------
    //for( int b = 2; b <= B; ++b )
    //{
    //    int d = 2;
    //    int maxF = F_max(d, b);         // F_max[GENERAL]
    //    for( int f = 2; f <= F; ++f )
    //    {
    //        if (f > maxF)
    //            maxF = F_max(++d, b);     // F_max[GENERAL]
    //        DROP[b-1][f-1] = d;
    //    }
    //}
    //return DROP[B-1][F-1];

    //----------------------------
    // basic N or rare N only. say such N are rare as i like Manhattan
    if (F <= 0) return 0;
    if (F == 1) return 1;

    if (B <= 0) return 0;
    if (B == 1) return (unsigned int) F;
    //if (B == 2) return ceil((sqrt(1.0 + 8*F) - 1)/2);
    //if (B == 3) return ???;


    int minD = DROP[B-1][F-1];
    if( minD >= 0 )
    {
        return minD;
    }

    minD = (int) F;
    for (int n = 1; n <= F; ++n)        // Drop from floor n.
    {
        int lower = Omlette(B-1, n-1);   // If it breaks, test floors 1 to n-1 with B-1 eggs.
        int upper = Omlette(B, F-n);     // If it doesn't break, test floors n+1 to F with B eggs.
        int drops = 1 + std::max(lower, upper);  // find worst case number of drops. since 1 drop already used.
        minD = std::min(drops, minD);
    }
    return DROP[B-1][F-1] = minD;
}

int PathToBreakFloor(int D, int B, int breakF, long long offsetF)
{
    long long maxF = F_max(--D, B-1) + 1;
    printf(" %D", maxF+offsetF);

    if (D <= 1 || B <= 1)
    {
        //return maxF+offsetF + B-1;
        return breakF;
    }

    if (maxF >= (breakF - offsetF))
    {
        return PathToBreakFloor(D, --B, breakF, offsetF);
    }

    offsetF += maxF;
    return PathToBreakFloor(D, B, breakF, offsetF);
}
#pragma endregion
// ------------------------------------------------------
#pragma region 2 - FLOOR[DROPS][BREAKS]

long long FLOOR[DROPS][BREAKS];

void InitFloor();
long long FF(unsigned int D, unsigned int B);

//               b  ┌ d ┐
// FLOOR(d, b) = Σ  │   │
//              i=1 └ i ┘
// FLOOR(d, b) = FLOOR(d-1, b-1) + FLOOR(d-1, b) + 1
// FLOOR(1, b) = 1 -> FLOOR(1, 1) = 1
// FLOOR(d, 0) = 0
// FLOOR(d, 1) = d
// FLOOR(d, 2) = d(d + 1)/2
// FLOOR(d, 3) = d(d(d + 0) + 5)/6;
// FLOOR(d, 4) = d(d(d(d - 2) + 11) + 14)/24;
// FLOOR(x, x) = 2^x - 1 = 1<<x - 1
// FLOOR(x, y) = FLOOR(x, x), if y >= x


void InitFloor()
{
    //for (int d = 1; d <= DROPS; ++d)
    //    for (int b = 1; b <= BREAKS; ++b)
    //        FLOOR[d - 1][b - 1] = -1; //0;

    for (int b = 1; b <= BREAKS; ++b)
    {
        FLOOR[1 - 1][b - 1] = 0x01LL;
    }
    //memset(FLOOR[1-1], 0x01LL, sizeof(FLOOR[1-1]));

    FF(DROPS, BREAKS);
}


long long FF(unsigned int D, unsigned int B)
{
    //if( D == 0 || B == 0 ) return 0;
    ////if( D <= B ) return (1LL << D) - 1;
    //if( FLOOR[D-1][B-1] == -1 ) FLOOR[D-1][B-1] = FF(D-1, B-1) + FF(D-1, B) + 1;
    //if( FLOOR[D-1][B-1] >= LLONG_MAX/2) FLOOR[D-1][B-1] = LLONG_MAX/2;
    //return FLOOR[D-1][B-1];


    for (int d = 2; d <= D; ++d)
    {
        int b;
        b = 1; FLOOR[d - 1][b - 1] = d;
        b = 2; FLOOR[d - 1][b - 1] = d * (d + 1) / 2;
        b = 3; FLOOR[d - 1][b - 1] = d * (d * (d + 0) + 5) / 6;
        b = 4; FLOOR[d - 1][b - 1] = d * (d * (d * (d - 2) + 11) + 14) / 24;
        for (++b; b <= B; ++b) // b = b+1
        {
            long long ybFloor = FLOOR[(d - 1) - 1][(b - 1) - 1];
            long long nbFloor = FLOOR[(d - 1) - 1][(b) - 1];

            if (ybFloor == OUT_FLOOR || nbFloor == OUT_FLOOR)
            {
                FLOOR[d - 1][b - 1] = OUT_FLOOR;
                continue;
            }

            long long nextFloor = 0;
            if (b < d)
            {
                nextFloor = ybFloor + nbFloor + 1LL;
            }
            else if (b == d)
            {
                nextFloor = (1LL << d) - 1LL; //pow(2.0, d) - 1LL;
            }
            else
            {
                nextFloor = FLOOR[d - 1][b - 1 - 1];
            }

            if (nextFloor >= LLONG_MAX / 2)
                nextFloor = OUT_FLOOR;

            FLOOR[d - 1][b - 1] = nextFloor;
        }
    }
    return FLOOR[D-1][B-1];
}
#pragma endregion
// ------------------------------------------------------
#pragma region Functions

long long F_max(unsigned int D, unsigned int B) 
{
    long long maxF;

    if (D > DROPS) return -1;
    if (B > BREAKS) B = BREAKS;

    if (B == 0) return 0;
    if (B == 1) return D;
    if (B == 2) return D * (D + 1) / 2;
    if (B == 3) return D * (D * (D + 0) + 5) / 6;
    if (B == 4) return D * (D * (D * (D - 2) + 11) + 14) / 24;

    // ---------------------------------GENERAL
    //maxF = 0;
    //while (B > 0)
    //    maxF += Combine(D, B--);
    // ---------------------------------
    maxF = 0;
    long long incF = 1;
    for (int b = 1; b <= B && incF != 0; ++b)
    {
        incF = incF * (D - b + 1) / b;
        maxF += incF;

        if (maxF >= LLONG_MAX / 2)
        {
            return OUT_FLOOR;
        }
    }
    // ---------------------------------DROP[B][F]
    //maxF = 0;
    //while (DROP[B-1][maxF] <= D)
    //    ++maxF;

    // ---------------------------------FLOOR[D][B]
    //maxF = FLOOR[D-1][B-1];
    // ---------------------------------

    return maxF;
}

unsigned int D_min(unsigned long long F, unsigned int maxD, unsigned int B)
{
    int minD;

    if (B == 1) return (unsigned int) F;

    // ---------------------------------GENERAL

    // ---------------------------------DROP[B][F]
    //minD = DROP[B-1][F-1];

    // ---------------------------------FLOOR[D][B]
    //for (minD = 1; minD <= maxD && F_max(minD, B) < F; ++minD);

    //for (minD = 1; minD <= maxD; ++minD) 
    //{
    //    long long maxF = F_max(minD, B);
    //    if (maxF >= F || maxF == OUT_FLOOR)
    //        break;
    //}

    int min = 0;
    int max = maxD; //INT_MAX;
    while (max - min > 1)
    {
        int mid = (max + min) / 2;
        unsigned long long maxF = F_max(mid, B);

        //if (maxF >= F || maxF == OUT_FLOOR)
        //    max = mid;
        //else
        //    min = mid;

        if (F > maxF)
        {
            min = mid;
        }
        else
        {
            max = mid;
        }

    }
    minD = max;

    // --------------------------------

    return minD;
}

unsigned int B_min(unsigned long long F, unsigned int D, unsigned int maxB) 
{
    int minB;
    
    // ---------------------------------GENERAL
    
    //minB = 0;
    //long long maxF = 0;
    //long long incF = 1;
    //for (int b = 1; /*maxF < F*/; ++b)
    //{
    //    incF = incF*(D-b+1)/b;
    //    maxF += incF;
    //    if (maxF >= F)
    //    {
    //        minB = b;
    //        break;
    //    }
    //}

    // ---------------------------------FLOOR[D][B]
    //for (minB = 1; minB <= maxB && F_max(D, minB) < F; ++minB); 

    //for (minB = 1; minB <= maxB; ++minB)
    //{
    //    long long maxF = F_max(D, minB);
    //    if (maxF >= F || maxF == OUT_FLOOR)
    //        break;
    //}

    int min = 0;
    int max = maxB; //INT_MAX;
    while (max - min > 1)
    {
        int mid = (max + min) / 2;
        unsigned long long maxF = F_max(D, mid);

        //if (maxF >= F || maxF == OUT_FLOOR)
        //    max = mid;
        //else
        //    min = mid;

        if (F > maxF)
        {
            min = mid;
        }
        else
        {
            max = mid;
        }
    }
    minB = max;

    // ---------------------------------

    return minB;
}

// ---

void FloorTable(unsigned int D, unsigned int B)
{
    printf("\n d\\e");
    for (int b = 1; b <= B; ++b)
    {
        printf("      %d", b);
    }
    printf("\n");
    printf("----");
    for (int b = 1; b <= B; ++b)
    {
        printf("-------");
    }
    printf("\n");
    for (int d = 1; d <= D; ++d)
    {
        printf("%3d|", d);
        for (int b = 1; b <= B && b <= d; ++b)
        {
            printf("  %5lld", F_max(d, b));
        }
        printf("\n");
    }
}
#pragma endregion
// ------------------------------------------------------

void main()
{
    //InitDrop();
    InitFloor();

    freopen("EggDrop.in", "r", stdin);
    freopen("EggDrop.out", "w", stdout);
    
    int N;
    scanf(" %d", &N);
    
    assert(1 <= N && N <= 100);

    for (int i = 1; i <= N; ++i)
    {
        int F, D, B;
        scanf(" %d %d %d", &F, &D, &B);

        //int Limit = 100;      //small
        int Limit = 2000000000; //large

        assert(1 <= F && F <= Limit);
        assert(1 <= D && D <= Limit);
        assert(1 <= B && B <= Limit);

        printf("%d: %lld %d %d\n",
            i, F_max(D, B), D_min(F, D, B), B_min(F, D, B));
    }
    //FloorTable(16, 8);
    //system("PAUSE");
}