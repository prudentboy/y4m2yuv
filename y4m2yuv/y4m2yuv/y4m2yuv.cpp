// y4m2yuv.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

struct Y4mInfo
{
	char *name;      //文件名
	char *format;    //文件格式
	int width;       //帧宽度
	int height;      //帧高度
	float FPS;      //帧率
	char *field;     //场模式    
	float pixelratio;  //像素宽高比
	char *colorspace;  //色彩空间
	char *comment;   //注释
	int headlen;     //头部长度
	int framenum;    //视频帧数
};

void y4m2yuv(char *url, char *url1, Y4mInfo info)
{
	FILE *fp=fopen(url,"rb+");
	FILE *fp1=fopen(url1,"wb+");

	unsigned char *pic=(unsigned char *)malloc(info.width*info.height*1.5);
	fread(pic,1,info.headlen,fp);

	for(int i=0;i<info.framenum;i++)
	{
		fread(pic,1,6,fp);
		fread(pic,1,info.width*info.height*1.5,fp);
		fwrite(pic,1,info.width*info.height*1.5,fp1);
	}

	free(pic);
	fclose(fp);
	fclose(fp1);

	return;
}

Y4mInfo readinfobytes(char *url, char *url1)
{
	FILE *fp=fopen(url,"rb+");
	FILE *fp1=fopen(url1,"wb+");

	Y4mInfo info;
	info.name=url;
	fwrite("File name: ",1,11,fp1);
	fwrite(info.name,1,strlen(url),fp1);
	fwrite("\r\n",1,2,fp1);

	unsigned char *head=(unsigned char *)malloc(100);
	unsigned char *tmp=(unsigned char *)malloc(20);
	int picmark=0;
	int tmplen=0;

	fread(head,1,100,fp);

	for(int i=0;i<100;i++)
	{
		if(head[i]!=' ' && head[i]!=0x0A)
		{
			tmp[tmplen]=head[i];
			tmplen++;
		}
		else
		{
			switch(tmp[0])
			{
			case 'Y':
				info.format=(char *)malloc(tmplen);
				memcpy(info.format,(const char *)tmp,tmplen);
				fwrite("File format: ",1,13,fp1);
				fwrite(info.format,1,tmplen,fp1);
				fwrite("\r\n",1,2,fp1);
				break;
			case 'W':
				info.width=0;
				for(int i=1;i<tmplen;i++)
					info.width+=(int)((tmp[i]-'0')*pow((float)10,(float)tmplen-1-i));
				fwrite("Frame width: ",1,13,fp1);
				fwrite(tmp+1,1,tmplen-1,fp1);
				fwrite("\r\n",1,2,fp1);
				break;
			case 'H':
				info.height=0;
				for(int i=1;i<tmplen;i++)
					info.height+=(int)((tmp[i]-'0')*pow((float)10,(float)tmplen-1-i));
				fwrite("Frame height: ",1,14,fp1);
				fwrite(tmp+1,1,tmplen-1,fp1);
				fwrite("\r\n",1,2,fp1);
				break;
			case 'F':
				float a,b;
				int j,k;
				a=0;b=0;
				for(j=1;j<tmplen;j++)
					if(tmp[j]==':') break;
				for(k=1;k<j;k++)
					a+=(tmp[k]-'0')*pow((float)10,(float)j-1-k);
				for(k=j+1;k<tmplen;k++)
					b+=(tmp[k]-'0')*pow((float)10,(float)tmplen-1-k);
				info.FPS=a/b;
				fwrite("FPS: ",1,5,fp1);
				fwrite(tmp+1,1,tmplen-1,fp1);
				fwrite("\r\n",1,2,fp1);
				break;			
			case 'I':
				info.field=(char *)malloc(tmplen);
				memcpy(info.field,(const char *)tmp,tmplen);
				fwrite("Field order: ",1,13,fp1);
				fwrite(tmp+1,1,tmplen-1,fp1);
				fwrite("\r\n",1,2,fp1);
				break;			
			case 'A':
				a=0;b=0;
				for(j=1;j<tmplen;j++)
					if(tmp[j]==':') break;
				for(k=1;k<j;k++)
					a+=(tmp[k]-'0')*pow((float)10,(float)j-1-k);
				for(k=j+1;k<tmplen;k++)
					b+=(tmp[k]-'0')*pow((float)10,(float)tmplen-1-k);
				info.pixelratio=a/b;
				fwrite("Pixel ratio: ",1,13,fp1);
				fwrite(tmp+1,1,tmplen-1,fp1);
				fwrite("\r\n",1,2,fp1);
				break;			
			case 'C':
				break;			
			case 'X':
				break;
			default:
				break;
			}
			tmplen=0;	
			if(head[i]==0x0A)
			{
				info.headlen=i+1;
				break;
			}
		}
	}
	free(head);

	info.framenum=0;
	float colorfactor=1.5;
	unsigned char *file=(unsigned char *)malloc((int)(info.width*info.height*colorfactor)+6);	
	while(!feof(fp))
	{
		fread(file,1,(int)(info.width*info.height*colorfactor)+6,fp);
		info.framenum++;
	}

	fwrite("Frame number: ",1,14,fp1);
	fprintf(fp1,"%d",info.framenum);
	fwrite("\r\n",1,2,fp1);
	free(file);
	free(tmp);

	fclose(fp);
	fclose(fp1);

	return info;
}

int _tmain(int argc, _TCHAR* argv[])
{
	Y4mInfo fileinfo;
	fileinfo=readinfobytes("akiyo_cif.y4m","akiyo_cif_info.txt");
	y4m2yuv("akiyo_cif.y4m","akiyo_cif.yuv",fileinfo);
	return 0;
}