//Nuestro Modulo
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/time.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>


static struct proc_dir_entry *proc_entry;
static unsigned long primo;
static char *input_primo;
int acum_lecturas = 0;


int cant_lecturas(char * page, char **start, off_t off, int count, int *eof, void *data)
{
    int len;
    len = sprintf(page, "Lecturas/Escrituras realizadas: %d\n", acum_lecturas);
    return len;
}


static ssize_t primos_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos){
    int cant=0;
    cant = copy_from_user(&input_primo, buf, len);
    if (cant != 0)
         return -EFAULT;
    cant=sscanf(&input_primo, "%lu", &primo);
    if (cant!=1) {
        printk(KERN_INFO "primo: Eror en la cantidad de variables\n");
        return -EINVAL;
    }
    *ppos = len;
    acum_lecturas++;
    return len;
}

int esPrimo_check(int p){
	int i;
    for(i = 2; i < p; i++)
		if(p%i == 0)
			return 0;
	return 1;
}
//Funciones de lectura invocada por /dev fs
static ssize_t primos_read(struct file *file, char *buf, size_t count, loff_t *ppos){
	
    int res= 0;
    char buffer[20];

    if (count < 1) return -EINVAL;
    if (*ppos !=0 ) return 0;

    res = esPrimo_check(primo);
    printk(KERN_INFO "esPrimo: %d\n", res);
    int len = sprintf(buffer, "%d", res);
    if (copy_to_user(buf, buffer,len))
        return -EINVAL;
    *ppos = len;
    acum_lecturas++;
    return len;
}

// Estructuras utililizadas por la funcion de registro de dispositivos
static const struct file_operations primos_fops = {
    .owner = THIS_MODULE,
    .read = primos_read,
    .write = primos_write
};

static struct miscdevice primos_dev = {
    MISC_DYNAMIC_MINOR,
    "esPrimo",
    &primos_fops
};

// Funciones utilizadas por la creacion y destruccion del modulo
static int __init primos_init(void) {
    int ret;
    // Registración del device
    ret = misc_register(&primos_dev);
    if (ret)
    {
        printk(KERN_ERR "No se puede registrar el dispositivo \n");
    } else
    {
 		//Iniciamos dispositivo
        primo = 0;
        input_primo= (char *) vmalloc(PAGE_SIZE);
        memset(input_primo, 0, PAGE_SIZE);


	    // Definicion de la entrada en /proc con la asociacion de funciones de lectura y escritura
	    proc_entry = create_proc_entry("primosCant", 0644, NULL);
	    if (proc_entry == NULL)
	    {
	        printk(KERN_INFO "primos: No se pudo crear la entrada en /proc\n");
	    }else
        {
             proc_entry->read_proc=cant_lecturas;
        }
    }
    return ret;
}

static void __exit primos_exit(void) {
    remove_proc_entry("primosCant", NULL);
    misc_deregister(&primos_dev);
}

// Definicion de constructor y destructor del modulo.
module_init(primos_init);
module_exit(primos_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Franco Negri, Ricardo Colombo");
MODULE_DESCRIPTION("Modulo de numeros primos");
