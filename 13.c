
void main(){


int i;
int n;
int t;
int v[100];
for(i=0;i<n/2;i=i+1){
		t=v[i];
		v[i]=v[n-i-1];
		v[n-i-1]=t;
}
}