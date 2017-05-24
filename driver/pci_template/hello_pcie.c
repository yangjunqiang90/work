#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/signal.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/pci.h>
#include <linux/interrupt.h> 
#include <asm/uaccess.h> 
#define DEV_NAME "hello_pcie"

#define DEBUG 

#ifdef DEBUG
	#define DEBUG_ERR(format,args...) \
	do{  \
		printk("[%s:%d] ",__FUNCTION__,__LINE__); \
		printk(format,##args); \
	}while(0)
#else
	#define DEBUG_PRINT(format,args...) 
#endif


#define FASYNC_MINOR 1
#define FASYNC_MAJOR 244
#define DEVICE_NUMBER 1
static struct class * hello_class;
static struct device * hello_class_dev;

struct hello_device
{
	struct pci_dev* pci_dev;
	struct cdev cdev;
	dev_t devno;
}my_device;

//barn(n=0,1,2����0��1��2��3��4��5) �ռ�������ַ�����ȣ������ַ
unsigned long bar0_phy;
unsigned long bar0_vir;
unsigned long bar0_length;

unsigned long bar1_phy;
unsigned long bar1_vir;
unsigned long bar1_length;

//1M 
#define DMA_BUFFER_SIZE 1*1024*1024  

//����DMAת��ʱ��dma��Դ��ַ��Ŀ�ĵ�ַ
dma_addr_t dma_src_phy;
dma_addr_t dma_src_vir;

dma_addr_t dma_dst_phy;
dma_addr_t dma_dst_vir;


//�����豸��id��д,������賧��id���豸id
#define HELLO_VENDOR_ID 0x666
#define HELLO_DEVICE_ID 0x999
static struct pci_device_id hello_ids[] = {
    {HELLO_VENDOR_ID,HELLO_DEVICE_ID,PCI_ANY_ID,PCI_ANY_ID,0,0,0},
    {0,}
};
MODULE_DEVICE_TABLE(pci,hello_ids);

static int hello_probe(struct pci_dev *pdev, const struct pci_device_id *id);
static void hello_remove(struct pci_dev *pdev);
static irqreturn_t hello_interrupt(int irq, void * dev);

//��iATUд���ݵĺ���
void iATU_write_config_dword(struct pci_dev *pdev,int offset,int value){
}

//������Ҫ��bar0ӳ�䵽�ڴ�
static void iATU_bar0(void){
	//���漸�������ֲ�����example
	//iATU_write_config_dword(my_device.pci_dev,iATU Lower Target Address ,xxx);//xxx��ʾ�ڴ��еĵ�ַ����bar0ӳ�䵽����ڴ�
	//iATU_write_config_dword(my_device.pci_dev,iATU Upper Target Address ,xxx);//xxx��ʾ�ڴ��еĵ�ַ����bar0ӳ�䵽����ڴ�

	//iATU_write_config_dword(my_device.pci_dev,iATU Control 1,0x0);//ӳ���ʱ�ڴ棬����д0x0
	//iATU_write_config_dword(my_device.pci_dev,iATU Control 2,xxx);//ʹ��ĳ��region����ʼ��ַת��
}


//��dma���üĴ����ж�д���ݵĺ����������ѵ�һ��dma�Ĵ�����Ѱַ��
int dma_read_config_dword(struct pci_dev *pdev,int offset){
	int value =0;
	return value;
}

void dma_write_config_dword(struct pci_dev *pdev,int offset,int value){
	
}

void dma_init(void){
	int pos;
	u16 msi_control;
	u32 msi_addr_l;
	u32 msi_addr_h;
	u32 msi_data;
	
	//1.dma ͨ��0 д��ʼ�� ����η���DMA global register �Ĵ�������Ҫ���ݾ����Ӳ��������ͨ��pci_write/read_config_word/dword��
	//Ҳ����ͨ��ĳ��bar������ͨ��bar0+ƫ�������ʡ�
	//1.1 DMA write engine enable =0x1������������Լ���оƬ��д
	//dma_write_config_dword(->pci_dev,DMA write engine enable,0x1);
	
	//1.2 ��ȡmsi�����Ĵ����ĵ�ַ
	pos =pci_find_capability(my_device.pci_dev,PCI_CAP_ID_MSI);
	//1.3 ��ȡmsi��Э�鲿�֣��õ�pci�豸��32λ����64λ����ͬ�ļܹ�msi data�Ĵ�����ַͬ
	pci_read_config_word(my_device.pci_dev,pos+2,&msi_control);
	//1.4 ��ȡmsi�����Ĵ������еĵ�ַ�Ĵ�����ֵ
	pci_read_config_dword(my_device.pci_dev,pos+4,&msi_addr_l);
	
	//1.5 ���� DMA write done IMWr Address Low.����������Լ���оƬ��д
	//dma_write_config_dword(my_device.pci_dev,DMA write done IMWr Address Low,msi_addr_l);
	//1.6 ���� DMA write abort IMWr Address Low.����������Լ���оƬ��д
	//dma_write_config_dword(my_device.pci_dev,DMA write abort IMWr Address Low,msi_addr_l);
	
	if(msi_control&0x80){
		//64λ��
		//1.7 ��ȡmsi�����Ĵ������еĸ�32λ��ַ�Ĵ�����ֵ
		pci_read_config_dword(my_device.pci_dev,pos+0x8,&msi_addr_h);
		//1.8 ��ȡmsi�����Ĵ������е����ݼĴ�����ֵ
		pci_read_config_dword(my_device.pci_dev,pos+0xc,&msi_data);
		
		//1.9 ���� DMA write done IMWr Address High.����������Լ���оƬ��д
		//dma_write_config_dword(my_device.pci_dev,DMA write done IMWr Address High,msi_addr_h);
		//1.10 ���� DMA write abort IMWr Address High.����������Լ���оƬ��д
		//dma_write_config_dword(my_device.pci_dev,DMA write abort IMWr Address High,msi_addr_h);
		
	}else{
		//1.11 ��ȡmsi�����Ĵ������е����ݼĴ�����ֵ
		pci_read_config_dword(my_device.pci_dev,pos+0x8,&msi_data);
	}
	
	//1.12 �����ݼĴ�����ֵд�뵽dma�Ŀ��ƼĴ������е� DMA write channel 0 IMWr data��
	//dma_write_config_dword(my_device.pci_dev,DMA write channel 0 IMWr data,msi_data);
	
	//1.13 DMA channel 0 control register 1 = 0x4000010
	//dma_write_config_dword(my_device.pci_dev,DMA channel 0 control register 1,0x4000010);
	
	//2.dma ͨ��0 ����ʼ�� �������������ƣ�����������
}

static int hello_probe(struct pci_dev *pdev, const struct pci_device_id *id){
	int i;
	int result;
	//ʹ��pci�豸
	if (pci_enable_device(pdev)){
        result = -EIO;
		goto end;
	}

	
	pci_set_master(pdev);
	
	my_device.pci_dev=pdev;

	if(unlikely(pci_request_regions(pdev,DEV_NAME))){
		DEBUG_ERR("failed:pci_request_regions\n");
		result = -EIO;
		goto enable_device_err;
	}
	
	//���bar0�������ַ�������ַ
	bar0_phy = pci_resource_start(pdev,0);
	if(bar0_phy<0){
		DEBUG_ERR("failed:pci_resource_start\n");
		result =-EIO;
		goto request_regions_err;
	}
	
	//����bar0����Ϊ�ڴ棬�����������ģ������ڱ������в���bar0�����κβ�����
	bar0_length = pci_resource_len(pdev,0);
	if(bar0_length!=0){
		bar0_vir = (unsigned long)ioremap(bar0_phy,bar0_length);
	}
	
	//����һ��DMA�ڴ棬��ΪԴ��ַ���ڽ���DMA��д��ʱ����õ���
	dma_src_vir=(dma_addr_t)pci_alloc_consistent(pdev,DMA_BUFFER_SIZE,&dma_src_phy);
	if(dma_src_vir != 0){
		for(i=0;i<DMA_BUFFER_SIZE/PAGE_SIZE;i++){
			SetPageReserved(virt_to_page(dma_src_phy+i*PAGE_SIZE));
		}
	}else{
		goto free_bar0;
	}
	
	//����һ��DMA�ڴ棬��ΪĿ�ĵ�ַ���ڽ���DMA��д��ʱ����õ���
	dma_dst_vir=(dma_addr_t)pci_alloc_consistent(pdev,DMA_BUFFER_SIZE,&dma_dst_phy);
	if(dma_dst_vir!=0){
		for(i=0;i<DMA_BUFFER_SIZE/PAGE_SIZE;i++){
			SetPageReserved(virt_to_page(dma_dst_phy+i*PAGE_SIZE));
		}
	}else{
		goto alloc_dma_src_err;
	}
	//ʹ��msi��Ȼ����ܵõ�pdev->irq
	 result = pci_enable_msi(pdev);
	 if (unlikely(result)){
		DEBUG_ERR("failed:pci_enable_msi\n");
		goto alloc_dma_dst_err;
    }
	
	result = request_irq(pdev->irq, hello_interrupt, 0, DEV_NAME, my_device.pci_dev);
    if (unlikely(result)){
       DEBUG_ERR("failed:request_irq\n");
	   goto enable_msi_error;
    }
	
	//DMA �Ķ�д��ʼ��
	dma_init();
	
enable_msi_error:
		pci_disable_msi(pdev);
alloc_dma_dst_err:
	for(i=0;i<DMA_BUFFER_SIZE/PAGE_SIZE;i++){
		ClearPageReserved(virt_to_page(dma_dst_phy+i*PAGE_SIZE));
	}
	pci_free_consistent(pdev,DMA_BUFFER_SIZE,(void *)dma_dst_vir,dma_dst_phy);
alloc_dma_src_err:
	for(i=0;i<DMA_BUFFER_SIZE/PAGE_SIZE;i++){
		ClearPageReserved(virt_to_page(dma_src_phy+i*PAGE_SIZE));
	}
	pci_free_consistent(pdev,DMA_BUFFER_SIZE,(void *)dma_src_vir,dma_src_phy);
free_bar0:
	iounmap((void *)bar0_vir);
request_regions_err:
	pci_release_regions(pdev);
	
enable_device_err:
	pci_disable_device(pdev);
end:
	return result;
}

static void hello_remove(struct pci_dev *pdev){
	int i;
	
	free_irq(pdev->irq,my_device.pci_dev);
	pci_disable_msi(pdev);

	for(i=0;i<DMA_BUFFER_SIZE/PAGE_SIZE;i++){
		ClearPageReserved(virt_to_page(dma_dst_phy+i*PAGE_SIZE));
	}
	pci_free_consistent(pdev,DMA_BUFFER_SIZE,(void *)dma_dst_vir,dma_dst_phy);

	for(i=0;i<DMA_BUFFER_SIZE/PAGE_SIZE;i++){
		ClearPageReserved(virt_to_page(dma_src_phy+i*PAGE_SIZE));
	}
	pci_free_consistent(pdev,DMA_BUFFER_SIZE,(void *)dma_src_vir,dma_src_phy);

	iounmap((void *)bar0_vir);
	pci_release_regions(pdev);
	pci_disable_device(pdev);
}

//�ѵ������ж���Ӧ����
static irqreturn_t hello_interrupt(int irq, void * dev){
	//1.���жϵ���ʱ������DMA��ɵ�ʱ�򣬻���msi_addr��д��msi_data,�Ӷ������жϵ����������
	//2.����DMA Channel control 1 register�Ĵ�����״̬���ж϶�д״̬����ʧ�ܣ�дʧ�ܣ����ɹ���д�ɹ���������ͬ�Ĵ���
	return 0;
}
static struct pci_driver hello_driver = {
    .name = DEV_NAME,
    .id_table = hello_ids,
    .probe = hello_probe,
    .remove = hello_remove,
};

static int hello_open(struct inode *inode, struct file *file)
{
	printk("driver: hello_open\n");
	//��д��Ʒ���߼�
	return 0;
}

int hello_close(struct inode *inode, struct file *file)
{
	printk("driver: hello_close\n");
	//��д��Ʒ���߼�
	return 0;
}

long hello_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	//���Ʒ���߼�
	//ΪӦ�ò��ṩ�ĺ����ӿڣ�ͨ������cmd����switch��������ͬ�Ĵ��� 
	iATU_bar0();//ĳ�����ʵĵط�����
	return 0;
	
}

static struct file_operations hello_fops = {
	.owner   		=  THIS_MODULE,    
	.open   		=  hello_open,     
	.release 		=  hello_close,
	.unlocked_ioctl =  hello_unlocked_ioctl,
	//�ѵ��������dma�Ķ�д��read��write����).
};



static int hello_drv_init(void)
{
	int ret;
	ret = pci_register_driver(&hello_driver);
	if (ret < 0) {
		printk("failed: pci_register_driver\n");
		return ret;
	}
	
	ret=alloc_chrdev_region(&my_device.devno,0,DEVICE_NUMBER,"hello");
	if (ret < 0) {
		printk("failed: register_chrdev_region\n");
		return ret;
	}

	cdev_init(&my_device.cdev, &hello_fops);
	ret = cdev_add(&my_device.cdev, my_device.devno, DEVICE_NUMBER);
	if (ret < 0) {
		printk("faield: cdev_add\n");
		return ret;
	}
	
	hello_class = class_create(THIS_MODULE, "hello_class");
	hello_class_dev = device_create(hello_class, NULL, my_device.devno, NULL, "hello_device"); 

	return 0;
}

static void hello_drv_exit(void)
{
	device_destroy(hello_class,my_device.devno);
	class_destroy(hello_class);
		
	cdev_del(&(my_device.cdev));
	unregister_chrdev_region(my_device.devno,DEVICE_NUMBER);
	pci_unregister_driver(&hello_driver);
}


module_init(hello_drv_init);
module_exit(hello_drv_exit);

MODULE_LICENSE("GPL");
