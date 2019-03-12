/*实验三: 
此次实验限定语言C/C++，允许使用stl。
ddl：下下周六（11月19日）。
实验要求：实现任意二进制文件压缩解压。
将哈夫曼树或者词频率表保存到文件，
压缩后解压所需信息全部从你自己压缩得到的文件中拿到。
检查:对于一般txt文档实现效果明显的压缩结果并正确解压7分，
大文件（几十兆以上）非文本文件正确压缩解压。（9分）报告1分。 
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

char *HuffmanCode[256];//用来存储每个字符的Huffman编码 
int weighttemp[256];//用来存储每个字符的权重 
char filename[1000];
char savename[1000];

void HTCreate(HTNode *node,int i){//从0到i-1节点中选出权值最小的两个节点 
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
	node[i].weight=node[a].weight+node[b].weight;//权值相加生成新节点 
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
	for(i=n;i<2*n-1;i++) HTCreate(node,i);//生成Huffman树
	for(i=0;i<2*n-1;i++) node[i].weight=0;//用weight域来标记遍历Huffman树过程中节点的状态 
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
		else{//weight=2的情况，代表此节点左右都已走过 
			node[k].weight=0;
			k=node[k].parent;
			cdlength--;
		}//退回一个节点，编码长度减一 
	}
	return node;
}

void Compress(){
	FILE *fpr,*fpw;
	char *write;
	unsigned char uch;
	HTNode *node;
	int Readbytecnt=0,Huffmanbytecnt=0;
	int Huffmancnt=0;//记录压缩前后的字节数
	int i,j,k,wch,filelength,rcnt=0,mark=0;//rcnt用于记录文档中出现的字符种类数量
	for(i=0;i<256;i++) weighttemp[i]=0;
	printf("请输入要压缩的文件名：");
	scanf("%s",filename);
	if(!(fpr=fopen(filename,"rb"))){
		printf("此文件无法打开，可能为空文件！");
		exit(0);
	}
	printf("请输入压缩文件的名字及格式：");
	scanf("%s",savename);
	if(!(fpw=fopen(savename,"wb"))){
		printf("此文件无法打开！");
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
	}//将字符频率表存入文件中 
	if(Huffmancnt%8){//若最后一位需要补齐 
		fputc(Huffmancnt%8,fpw);
	}
	else fputc(8,fpw); //不需补齐的情况 
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
	printf("文件压缩成功！");
	printf("文件压缩率为：%f%%",(float)(100*Huffmanbytecnt)/Readbytecnt);
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
	printf("请输入要解压的文件名称：");
	scanf("%s",filename);
	if(!(fpr=fopen(filename,"rb"))){
		printf("此文件无法打开！");
		exit(0);
	}
	printf("请输入解压文件的名字及格式：");
	scanf("%s",savename);
	if(!(fpw=fopen(savename,"wb"))){
		printf("此文件无法打开！");
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
	for(i=rcnt;i<2*rcnt-1;i++) HTCreate(node,i);//重新生成Huffman树
	ch=fgetc(fpr);
	valid=ch;//读取最后一个字节实际有效的位数 
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
			if(byte[i]){//如果编码为1，向右 
				k=node[k].rchild;
				if(node[k].lchild==-1&&node[k].rchild==-1){//如果走到了叶子节点，得到编码对应的字符并写入文件中 
					fwrite(&node[k].ascii,sizeof(char),1,fpw);
					k=2*rcnt-2;
				}
			}
			else{//如果编码为0，向左 
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
		if(byte[i]){//如果编码为1，向右 
			k=node[k].rchild;
			if(node[k].lchild==-1&&node[k].rchild==-1){//如果走到了叶子节点，得到编码对应的字符并写入文件中 
				fwrite(&node[k].ascii,sizeof(char),1,fpw);
				k=2*rcnt-2;
			}
		}
		else{//如果编码为0，向左 
			k=node[k].lchild;
			if(node[k].lchild==-1&&node[k].rchild==-1){ 
				fwrite(&node[k].ascii,sizeof(char),1,fpw);
				k=2*rcnt-2;
			}
		}
	}
	printf("解压完成！");
	fclose(fpr);
	fclose(fpw);
}

int main(){
	int sel;
	printf("请输入数字选择功能(0代表编码压缩，1代表解码解压)：\n");
	scanf("%d",&sel);
	if(!sel) Compress();
	else DeCompress();
	return 0;
}
