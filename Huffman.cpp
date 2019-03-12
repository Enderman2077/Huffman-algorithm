/*ʵ����: 
�˴�ʵ���޶�����C/C++������ʹ��stl��
ddl������������11��19�գ���
ʵ��Ҫ��ʵ������������ļ�ѹ����ѹ��
�������������ߴ�Ƶ�ʱ��浽�ļ���
ѹ�����ѹ������Ϣȫ�������Լ�ѹ���õ����ļ����õ���
���:����һ��txt�ĵ�ʵ��Ч�����Ե�ѹ���������ȷ��ѹ7�֣�
���ļ�����ʮ�����ϣ����ı��ļ���ȷѹ����ѹ����9�֣�����1�֡� 
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#define INFINITE 2147483647
typedef struct HTNode{
	int weight;
	int ascii;
	int parent,lchild,rchild;
}HTNode;

char *HuffmanCode[256];//�����洢ÿ���ַ���Huffman���� 
int weighttemp[256];//�����洢ÿ���ַ���Ȩ�� 
char filename[1000];
char savename[1000];

void HTCreate(HTNode *node,int i){//��0��i-1�ڵ���ѡ��Ȩֵ��С�������ڵ� 
	int min=INFINITE;
	int a,b,j;
	for(j=0;j<i;j++){
		if(node[j].parent!=-1)continue;
		if(node[j].weight<=min){
			a=j;
			min=node[j].weight;
		}
	}
	min=INFINITE;
	for(j=0;j<i;j++){
		if(j==a||node[j].parent!=-1)continue;
		if(node[j].weight<=min){
			b=j;
			min=node[j].weight;
		}
	}
	node[a].parent=i;
	node[b].parent=i;
	node[i].lchild=a;
	node[i].rchild=b;
	node[i].weight=node[a].weight+node[b].weight;//Ȩֵ��������½ڵ� 
}

HTNode *HuffmanCoding(int n){
	int i,j,k,cdlength=0;
	char cdtemp[n];	
	HTNode *node;
	node=(HTNode*)malloc((2*n-1)*sizeof(HTNode));
	for(i=0;i<2*n-1;i++){
		node[i].ascii=-1;
		node[i].lchild=-1;
		node[i].parent=-1;
		node[i].rchild=-1;
		node[i].weight=0;
	}
	j=0;
	for(i=0;i<256;i++){
		if(weighttemp[i]!=0){
			node[j].ascii=i;
			node[j].weight=weighttemp[i];
			j++;
		}
	}
	for(i=n;i<2*n-1;i++) HTCreate(node,i);//����Huffman��
	for(i=0;i<2*n-1;i++) node[i].weight=0;//��weight������Ǳ���Huffman�������нڵ��״̬ 
	k=2*n-2;
	while(k!=-1){
		if(node[k].weight==0){
			node[k].weight=1;
			if(node[k].lchild!=-1){
				k=node[k].lchild;
				cdtemp[cdlength]='0';
				cdlength++;
			}
			else if(node[k].rchild==-1){
				HuffmanCode[node[k].ascii]=(char *)malloc((cdlength+1)*sizeof(char));
				cdtemp[cdlength]='\0';
				strcpy(HuffmanCode[node[k].ascii],&cdtemp[0]);
			}
		}
		else if(node[k].weight==1){
			node[k].weight=2;
			if(node[k].rchild!=-1){
				k=node[k].rchild;
				cdtemp[cdlength]='1';
				cdlength++;
			}
		}
		else{//weight=2�����������˽ڵ����Ҷ����߹� 
			node[k].weight=0;
			k=node[k].parent;
			cdlength--;
		}//�˻�һ���ڵ㣬���볤�ȼ�һ 
	}
	return node;
}

void Compress(){
	FILE *fpr,*fpw;
	char *write;
	unsigned char uch;
	HTNode *node;
	int Readbytecnt=0,Huffmanbytecnt=0;
	int Huffmancnt=0;//��¼ѹ��ǰ����ֽ���
	int i,j,k,wch,filelength,rcnt=0,mark=0;//rcnt���ڼ�¼�ĵ��г��ֵ��ַ���������
	for(i=0;i<256;i++) weighttemp[i]=0;
	printf("������Ҫѹ�����ļ�����");
	scanf("%s",filename);
	if(!(fpr=fopen(filename,"rb"))){
		printf("���ļ��޷��򿪣�����Ϊ���ļ���");
		exit(0);
	}
	printf("������ѹ���ļ������ּ���ʽ��");
	scanf("%s",savename);
	if(!(fpw=fopen(savename,"wb"))){
		printf("���ļ��޷��򿪣�");
		exit(0);
	}
	fseek(fpr,0,SEEK_END);
	filelength=ftell(fpr);
	rewind(fpr);
	for(i=0;i<filelength;i++){
		fread(&uch,1,sizeof(char),fpr);
		weighttemp[uch]++;
		Readbytecnt++;
	}
	for(i=0;i<256;i++)
		if(weighttemp[i]!=0)rcnt++;
	node=HuffmanCoding(rcnt); 
	rewind(fpr);
	for(i=0;i<filelength;i++){
		fread(&uch,1,sizeof(char),fpr);
		Huffmancnt+=strlen(HuffmanCode[uch]);
	}
	write=(char *)malloc((Huffmancnt+1)*sizeof(char));
	rewind(fpr);
	for(k=0;k<filelength;k++){
		fread(&uch,1,sizeof(char),fpr);
		for(i=mark,j=0;i<mark+strlen(HuffmanCode[uch]);i++,j++) write[i]=HuffmanCode[uch][j];
		mark+=strlen(HuffmanCode[uch]);
	}
	write[Huffmancnt]='\0';
	
	fwrite(&rcnt,1,sizeof(int),fpw);
	for(i=0;i<rcnt;i++){
		fputc(node[i].ascii,fpw);
		fwrite(&weighttemp[node[i].ascii],sizeof(int),1,fpw);
	}//���ַ�Ƶ�ʱ�����ļ��� 
	if(Huffmancnt%8){//�����һλ��Ҫ���� 
		fputc(Huffmancnt%8,fpw);
	}
	else fputc(8,fpw); //���貹������ 
	mark=0;
	while(1){
		wch=0;
		for(i=mark,j=7;i<mark+8;i++,j--) wch+=pow(2,j)*(write[i]-48);
		Huffmanbytecnt++;
		fputc(wch,fpw);
		mark+=8;
		Huffmancnt-=8;
		if(Huffmancnt<8)break;
	}
	if(Huffmancnt){
		wch=0;
		for(i=0;i<Huffmancnt;i++) wch+=pow(2,Huffmancnt-i-1)*(write[i+mark]-48);
		Huffmanbytecnt++;
		fputc(wch,fpw);
	}
	fclose(fpr);
	fclose(fpw);
	printf("�ļ�ѹ���ɹ���");
	printf("�ļ�ѹ����Ϊ��%f%%",(float)(100*Huffmanbytecnt)/Readbytecnt);
	free(node);
	free(write);
}

void DeCompress(){
	FILE *fpr,*fpw;
	int rcnt,i,j,k,p,valid,strlength,mark,a,b,pos1,pos2;
	int byte[8];
	char ch;
	unsigned char uch;
	HTNode *node;
	printf("������Ҫ��ѹ���ļ����ƣ�");
	scanf("%s",filename);
	if(!(fpr=fopen(filename,"rb"))){
		printf("���ļ��޷��򿪣�");
		exit(0);
	}
	printf("�������ѹ�ļ������ּ���ʽ��");
	scanf("%s",savename);
	if(!(fpw=fopen(savename,"wb"))){
		printf("���ļ��޷��򿪣�");
		exit(0);
	}
	fseek(fpr,0,SEEK_END);
	pos2=ftell(fpr);
	rewind(fpr);
	fread(&rcnt,sizeof(int),1,fpr);
	node=(HTNode*)malloc((2*rcnt-1)*sizeof(HTNode));
	for(i=0;i<2*rcnt-1;i++){
		node[i].ascii=-1;
		node[i].lchild=-1;
		node[i].parent=-1;
		node[i].rchild=-1;
		node[i].weight=0;
	}
	for(i=0;i<rcnt;i++){
		fread(&uch,sizeof(char),1,fpr);
		node[i].ascii=uch;
		fread(&node[i].weight,sizeof(int),1,fpr);
	}
	for(i=rcnt;i<2*rcnt-1;i++) HTCreate(node,i);//��������Huffman��
	ch=fgetc(fpr);
	valid=ch;//��ȡ���һ���ֽ�ʵ����Ч��λ�� 
	k=2*rcnt-2;
	pos1=ftell(fpr);
	for(p=pos1;p<pos2-1;p++){
		fread(&uch,sizeof(char),1,fpr);
		for(j=0,i=uch;j<8;j++){
			a=i/2;
			b=i%2;
			i=a;
			byte[7-j]=b;
		}
		for(i=0;i<8;i++){
			if(byte[i]){//�������Ϊ1������ 
				k=node[k].rchild;
				if(node[k].lchild==-1&&node[k].rchild==-1){//����ߵ���Ҷ�ӽڵ㣬�õ������Ӧ���ַ���д���ļ��� 
					fwrite(&node[k].ascii,sizeof(char),1,fpw);
					k=2*rcnt-2;
				}
			}
			else{//�������Ϊ0������ 
				k=node[k].lchild;
				if(node[k].lchild==-1&&node[k].rchild==-1){ 
					fwrite(&node[k].ascii,sizeof(char),1,fpw);
					k=2*rcnt-2;
				}
			}
		}
	}
	fread(&uch,sizeof(char),1,fpr);
	for(j=0,i=uch;j<valid;j++){
		a=i/2;
		b=i%2;
		i=a;
		byte[7-j]=b;
	}
	for(i=8-valid;i<8;i++){
		if(byte[i]){//�������Ϊ1������ 
			k=node[k].rchild;
			if(node[k].lchild==-1&&node[k].rchild==-1){//����ߵ���Ҷ�ӽڵ㣬�õ������Ӧ���ַ���д���ļ��� 
				fwrite(&node[k].ascii,sizeof(char),1,fpw);
				k=2*rcnt-2;
			}
		}
		else{//�������Ϊ0������ 
			k=node[k].lchild;
			if(node[k].lchild==-1&&node[k].rchild==-1){ 
				fwrite(&node[k].ascii,sizeof(char),1,fpw);
				k=2*rcnt-2;
			}
		}
	}
	printf("��ѹ��ɣ�");
	fclose(fpr);
	fclose(fpw);
}

int main(){
	int sel;
	printf("����������ѡ����(0�������ѹ����1��������ѹ)��\n");
	scanf("%d",&sel);
	if(!sel) Compress();
	else DeCompress();
	return 0;
}
