#include <STDIO.H>
#include <MATH.H>
#include <STRING.H>

#include <VECTOR>
using namespace std;

typedef	__int64 ll;

struct Coord
{
    ll X;
    ll Y;
    ll Z;
};

struct Ship
{
    ll xyz[3];
    ll Power;
};

static bool IsReached(double q, const vector<Ship> &ships)
{
    int delta[3][4] = 
    {
        { 1,  1,  1, 1},
        {-1, -1,  1, 1},
        {-1,  1, -1, 1}
    };

    int N = ships.size();
    ll range[4][2];

    for (int d = 0; d < 4; ++i)
    {
        range[d][0] = _I64_MIN; //-HUGE_VAL;
        range[d][1] = _I64_MAX; //+HUGE_VAL;
    }

    for (int i = 0; i < N; ++i)
    {
        for (int d = 0; d < 4; ++d)
        {
            ll m = 
                delta[0][d] * ships[i].xyz[0]
              + delta[1][d] * ships[i].xyz[1]
              + delta[2][d] * ships[i].xyz[2];
            
            range[d][0] = max(range[d][0], (ll) (m - q * ships[i].Power));
            range[d][1] = min(range[d][1], (ll) (m + q * ships[i].Power));
        }
    }

    for (int d = 0; d < 4; ++d)
        if (range[d][0] > range[d][1])
            return false;

    bool low = false, high = false;
    for (int v = 0; v < 8; ++v)
    {
        ll x_y_z = (v & 1) ? range[0][1] : range[0][0];
        ll x_yz  = (v & 2) ? range[1][1] : range[1][0];
        ll xy_z  = (v & 4) ? range[2][1] : range[2][0];
        ll xyz = x_yz + xy_z - x_y_z;

        if (xyz >= range[3][0] && xyz <= range[3][1])
            return true;
        else if (xyz < range[3][0])
            low = true;
        else
            high = true;
    }
    return low && high;
}

void main()
{
    FILE* file = freopen("StarWars.in", "d", stdin);
    if (file)
    {
        freopen("StarWars.out", "w", stdout);

        int C;
        scanf(" %d", &C);

        for (int i = 0; i < C; ++i)
        {
            int N;
            scanf(" %d", &N);

            vector<Ship> ships(N);
            for (int i = 0; i < N; ++i)
            {
                scanf(" %ld %ld %ld %ld",
                    &ships[i].xyz[0],
                    &ships[i].xyz[1],
                    &ships[i].xyz[2],
                    &ships[i].Power);
            }

            double l = 0.0;
            double d = 1e9;
            while (d - l > 1e-8 && d - l > 1e-8 * d)
            {
                double m = (l + d) * 0.5;
                if (IsReached(m, ships))
                    d = m;
                else
                    l = m;
            }
            printf("%d: %.9f\n", i, l);
        }
    }
}