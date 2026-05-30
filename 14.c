void main()
{
    int i;
    int n;
    double s;

    s=0.0;

    put_s("n=");
    n=get_i();

    i=0;

    while(i<n){
        s=s+get_i();
        i=i+1;
    }

    put_s("mean=");
    put_d(s/n);
}