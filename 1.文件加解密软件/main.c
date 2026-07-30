#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 1024 //一次读取1024字节
#define PWD_MAX 64 //密码最长64字符
#define PATH_MAX 256 //文件路径（地址）最长256字符

int  XorEncryptFile(const char* srcPath, const char* dstPath, const char* pwd) {
	FILE* fpSrc = fopen(srcPath, "rb"); //二进制只读打开源文件
	if(fpSrc == NULL){
		printf("错误，找不到源文件\n");
		return -1;
	}
	FILE* fpDst = fopen(dstPath, "wb"); //二进制只写创建输出文件
	if (fpDst == NULL) {
		printf("错误，无法创建输出文件\n");
		fclose(fpSrc); //关闭源文件
		return -2;
	}
	int pwdlen = strlen(pwd); //字符串实际字符个数
	unsigned char buf[BUF_SIZE]; //缓冲区
	long readNum; //本次read到了第几字节
	int pwdIndex = 0; //读取的密码位
	while ((readNum = fread(buf, 1, BUF_SIZE, fpSrc) )> 0) {
		for (long i = 0; i < readNum;i++) {
			buf[i] = buf[i] ^ pwd[pwdIndex]; //异或计算
			pwdIndex = (pwdIndex+1) % pwdlen;
		}
		fwrite(buf, 1, readNum, fpDst);
	}
	fclose(fpSrc);
	fclose(fpDst);
	printf("文件处理完成\n");
	return 0;
}

void Menu() {
	printf("====文件异或加密软件====\n");
	printf("1 加密/解密\n");
	printf("2 退出\n");
	printf("请选择功能序号：\n");
}

int main() {
	int select;
	char password[PWD_MAX];
	char srcFile[PATH_MAX];
	char dstFile[PATH_MAX];
	while (1) { //循环菜单
		Menu();
		scanf_s("%d", &select);
		getchar();
		if (select == 2) {
			printf("退出\n");
			break;
		}
		else if (select == 1) {
			printf("请输入密码\n");
			fgets(password, PWD_MAX, stdin);
			password[strcspn(password, "\n")] = '\0';

			printf("请输入源文件路径:\n");
			fgets(srcFile, PATH_MAX, stdin);
			srcFile[strcspn(srcFile, "\n")] = '\0';

			printf("请输入输出文件路径:\n");
			fgets(dstFile, PATH_MAX, stdin);
			dstFile[strcspn(dstFile, "\n")] = '\0';

			XorEncryptFile(srcFile, dstFile, password);
		}
		else
			printf("输入错误，请重新输入\n");
		printf("\n");
	}
	return 0;
}