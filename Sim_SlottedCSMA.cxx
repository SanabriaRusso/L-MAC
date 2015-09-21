
#line 1 "Sim_SlottedCSMA.cc"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>


#line 1 "./COST/cost.h"

























#ifndef queue_t
#define queue_t SimpleQueue
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <deque>
#include <vector>
#include <assert.h>


#line 1 "./COST/priority_q.h"























#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H
#include <stdio.h>
#include <string.h>














template < class ITEM >
class SimpleQueue 
{
 public:
  SimpleQueue() :m_head(NULL) {};
  void EnQueue(ITEM*);
  ITEM* DeQueue();
  void Delete(ITEM*);
  ITEM* NextEvent() const { return m_head; };
  const char* GetName();
 protected:
  ITEM* m_head;
};

template <class ITEM>
const char* SimpleQueue<ITEM>::GetName()
{
  static const char* name = "SimpleQueue";
  return name;
}

template <class ITEM>
void SimpleQueue<ITEM>::EnQueue(ITEM* item)
{
  if( m_head==NULL || item->time < m_head->time )
  {
    if(m_head!=NULL)m_head->prev=item;
    item->next=m_head;
    m_head=item;
    item->prev=NULL;
    return;
  }
    
  ITEM* i=m_head;
  while( i->next!=NULL && item->time > i->next->time)
    i=i->next;
  item->next=i->next;
  if(i->next!=NULL)i->next->prev=item;
  i->next=item;
  item->prev=i;

}

template <class ITEM>
ITEM* SimpleQueue<ITEM> ::DeQueue()
{
  if(m_head==NULL)return NULL;
  ITEM* item = m_head;
  m_head=m_head->next;
  if(m_head!=NULL)m_head->prev=NULL;
  return item;
}

template <class ITEM>
void SimpleQueue<ITEM>::Delete(ITEM* item)
{
  if(item==NULL) return;

  if(item==m_head)
  {
    m_head=m_head->next;
    if(m_head!=NULL)m_head->prev=NULL;
  }
  else
  {
    item->prev->next=item->next;
    if(item->next!=NULL)
      item->next->prev=item->prev;
  }

}

template <class ITEM>
class GuardedQueue : public SimpleQueue<ITEM>
{
 public:
  void Delete(ITEM*);
  void EnQueue(ITEM*);
  bool Validate(const char*);
};
template <class ITEM>
void GuardedQueue<ITEM>::EnQueue(ITEM* item)
{

  ITEM* i=SimpleQueue<ITEM>::m_head;
  while(i!=NULL)
  {
    if(i==item)
    {
      pthread_printf("queue error: item %f(%p) is already in the queue\n",item->time,item);
    }
    i=i->next;
  }
  SimpleQueue<ITEM>::EnQueue(item);
}

template <class ITEM>
void GuardedQueue<ITEM>::Delete(ITEM* item)
{
  ITEM* i=SimpleQueue<ITEM>::m_head;
  while(i!=item&&i!=NULL)
    i=i->next;
  if(i==NULL)
    pthread_printf("error: cannot find the to-be-deleted event %f(%p)\n",item->time,item);
  else
    SimpleQueue<ITEM>::Delete(item);
}

template <class ITEM>
bool GuardedQueue<ITEM>::Validate(const char* s)
{
  char out[1000],buff[100];

  ITEM* i=SimpleQueue<ITEM>::m_head;
  bool qerror=false;

  sprintf(out,"queue error %s : ",s);
  while(i!=NULL)
  {
    sprintf(buff,"%f ",i->time);
    strcat(out,buff);
    if(i->next!=NULL)
      if(i->next->prev!=i)
      {
	qerror=true;
	sprintf(buff," {broken} ");
	strcat(out,buff);
      }
    if(i==i->next)
    {
      qerror=true;
      sprintf(buff,"{loop}");
      strcat(out,buff);
      break;
    }
    i=i->next;
  }
  if(qerror)
    printf("%s\n",out);
  return qerror;
}

template <class ITEM>
class ErrorQueue : public SimpleQueue<ITEM>
{
 public:
  ITEM* DeQueue(double);
  const char* GetName();
};

template <class ITEM>
const char* ErrorQueue<ITEM>::GetName()
{
  static const char* name = "ErrorQueue";
  return name;
}

template <class ITEM>
ITEM* ErrorQueue<ITEM> ::DeQueue(double stoptime)
{
  

  if(drand48()>0.5)
    return SimpleQueue<ITEM>::DeQueue();

  int s=0;
  ITEM* e;
  e=SimpleQueue<ITEM>::m_head;
  while(e!=NULL&&e->time<stoptime)
  {
    s++;
    e=e->next;
  }
  e=SimpleQueue<ITEM>::m_head;
  s=(int)(s*drand48());
  while(s!=0)
  {
    e=e->next;
    s--;
  }
  Delete(e);
  return e;
}

template < class ITEM >
class HeapQueue 
{
 public:
  HeapQueue();
  ~HeapQueue();
  void EnQueue(ITEM*);
  ITEM* DeQueue();
  void Delete(ITEM*);
  const char* GetName();
  ITEM* NextEvent() const { return num_of_elems?elems[0]:NULL; };
 private:
  void SiftDown(int);
  void PercolateUp(int);
  void Validate(const char*);
        
  ITEM** elems;
  int num_of_elems;
  int curr_max;
};

template <class ITEM>
const char* HeapQueue<ITEM>::GetName()
{
  static const char* name = "HeapQueue";
  return name;
}

template <class ITEM>
void HeapQueue<ITEM>::Validate(const char* s)
{
  int i,j;
  char out[1000],buff[100];
  for(i=0;i<num_of_elems;i++)
    if(  ((2*i+1)<num_of_elems&&elems[i]->time>elems[2*i+1]->time) ||
	 ((2*i+2)<num_of_elems&&elems[i]->time>elems[2*i+2]->time) )
    {
      sprintf(out,"queue error %s : ",s);
      for(j=0;j<num_of_elems;j++)
      {
	if(i!=j)
	  sprintf(buff,"%f(%d) ",elems[j]->time,j);
	else
	  sprintf(buff,"{%f(%d)} ",elems[j]->time,j);
	strcat(out,buff);
      }
      printf("%s\n",out);
    }
}
template <class ITEM>
HeapQueue<ITEM>::HeapQueue()
{
  curr_max=16;
  elems=new ITEM*[curr_max];
  num_of_elems=0;
}
template <class ITEM>
HeapQueue<ITEM>::~HeapQueue()
{
  delete [] elems;
}
template <class ITEM>
void HeapQueue<ITEM>::SiftDown(int node)
{
  if(num_of_elems<=1) return;
  int i=node,k,c1,c2;
  ITEM* temp;
        
  do{
    k=i;
    c1=c2=2*i+1;
    c2++;
    if(c1<num_of_elems && elems[c1]->time < elems[i]->time)
      i=c1;
    if(c2<num_of_elems && elems[c2]->time < elems[i]->time)
      i=c2;
    if(k!=i)
    {
      temp=elems[i];
      elems[i]=elems[k];
      elems[k]=temp;
      elems[k]->pos=k;
      elems[i]->pos=i;
    }
  }while(k!=i);
}
template <class ITEM>
void HeapQueue<ITEM>::PercolateUp(int node)
{
  int i=node,k,p;
  ITEM* temp;
        
  do{
    k=i;
    if( (p=(i+1)/2) != 0)
    {
      --p;
      if(elems[i]->time < elems[p]->time)
      {
	i=p;
	temp=elems[i];
	elems[i]=elems[k];
	elems[k]=temp;
	elems[k]->pos=k;
	elems[i]->pos=i;
      }
    }
  }while(k!=i);
}

template <class ITEM>
void HeapQueue<ITEM>::EnQueue(ITEM* item)
{
  if(num_of_elems>=curr_max)
  {
    curr_max*=2;
    ITEM** buffer=new ITEM*[curr_max];
    for(int i=0;i<num_of_elems;i++)
      buffer[i]=elems[i];
    delete[] elems;
    elems=buffer;
  }
        
  elems[num_of_elems]=item;
  elems[num_of_elems]->pos=num_of_elems;
  num_of_elems++;
  PercolateUp(num_of_elems-1);
}

template <class ITEM>
ITEM* HeapQueue<ITEM>::DeQueue()
{
  if(num_of_elems<=0)return NULL;
        
  ITEM* item=elems[0];
  num_of_elems--;
  elems[0]=elems[num_of_elems];
  elems[0]->pos=0;
  SiftDown(0);
  return item;
}

template <class ITEM>
void HeapQueue<ITEM>::Delete(ITEM* item)
{
  int i=item->pos;

  num_of_elems--;
  elems[i]=elems[num_of_elems];
  elems[i]->pos=i;
  SiftDown(i);
  PercolateUp(i);
}



#define CQ_MAX_SAMPLES 25

template <class ITEM>
class CalendarQueue 
{
 public:
  CalendarQueue();
  const char* GetName();
  ~CalendarQueue();
  void enqueue(ITEM*);
  ITEM* dequeue();
  void EnQueue(ITEM*);
  ITEM* DeQueue();
  ITEM* NextEvent() const { return m_head;}
  void Delete(ITEM*);
 private:
  long last_bucket,number_of_buckets;
  double bucket_width;
        
  void ReSize(long);
  double NewWidth();

  ITEM ** buckets;
  long total_number;
  double bucket_top;
  long bottom_threshold;
  long top_threshold;
  double last_priority;
  bool resizable;

  ITEM* m_head;
  char m_name[100];
};


template <class ITEM>
const char* CalendarQueue<ITEM> :: GetName()
{
  sprintf(m_name,"Calendar Queue (bucket width: %.2e, size: %ld) ",
	  bucket_width,number_of_buckets);
  return m_name;
}
template <class ITEM>
CalendarQueue<ITEM>::CalendarQueue()
{
  long i;
        
  number_of_buckets=16;
  bucket_width=1.0;
  bucket_top=bucket_width;
  total_number=0;
  last_bucket=0;
  last_priority=0.0;
  top_threshold=number_of_buckets*2;
  bottom_threshold=number_of_buckets/2-2;
  resizable=true;
        
  buckets= new ITEM*[number_of_buckets];
  for(i=0;i<number_of_buckets;i++)
    buckets[i]=NULL;
  m_head=NULL;

}
template <class ITEM>
CalendarQueue<ITEM>::~CalendarQueue()
{
  delete [] buckets;
}
template <class ITEM>
void CalendarQueue<ITEM>::ReSize(long newsize)
{
  long i;
  ITEM** old_buckets=buckets;
  long old_number=number_of_buckets;
        
  resizable=false;
  bucket_width=NewWidth();
  buckets= new ITEM*[newsize];
  number_of_buckets=newsize;
  for(i=0;i<newsize;i++)
    buckets[i]=NULL;
  last_bucket=0;
  total_number=0;

  
        
  ITEM *item;
  for(i=0;i<old_number;i++)
  {
    while(old_buckets[i]!=NULL)
    {
      item=old_buckets[i];
      old_buckets[i]=item->next;
      enqueue(item);
    }
  }
  resizable=true;
  delete[] old_buckets;
  number_of_buckets=newsize;
  top_threshold=number_of_buckets*2;
  bottom_threshold=number_of_buckets/2-2;
  bucket_top=bucket_width*((long)(last_priority/bucket_width)+1)+bucket_width*0.5;
  last_bucket = long(last_priority/bucket_width) % number_of_buckets;

}
template <class ITEM>
ITEM* CalendarQueue<ITEM>::DeQueue()
{
  ITEM* head=m_head;
  m_head=dequeue();
  return head;
}
template <class ITEM>
ITEM* CalendarQueue<ITEM>::dequeue()
{
  long i;
  for(i=last_bucket;;)
  {
    if(buckets[i]!=NULL&&buckets[i]->time<bucket_top)
    {
      ITEM * item=buckets[i];
      buckets[i]=buckets[i]->next;
      total_number--;
      last_bucket=i;
      last_priority=item->time;
                        
      if(resizable&&total_number<bottom_threshold)
	ReSize(number_of_buckets/2);
      item->next=NULL;
      return item;
    }
    else
    {
      i++;
      if(i==number_of_buckets)i=0;
      bucket_top+=bucket_width;
      if(i==last_bucket)
	break;
    }
  }
        
  
  long smallest;
  for(smallest=0;smallest<number_of_buckets;smallest++)
    if(buckets[smallest]!=NULL)break;

  if(smallest >= number_of_buckets)
  {
    last_priority=bucket_top;
    return NULL;
  }

  for(i=smallest+1;i<number_of_buckets;i++)
  {
    if(buckets[i]==NULL)
      continue;
    else
      if(buckets[i]->time<buckets[smallest]->time)
	smallest=i;
  }
  ITEM * item=buckets[smallest];
  buckets[smallest]=buckets[smallest]->next;
  total_number--;
  last_bucket=smallest;
  last_priority=item->time;
  bucket_top=bucket_width*((long)(last_priority/bucket_width)+1)+bucket_width*0.5;
  item->next=NULL;
  return item;
}
template <class ITEM>
void CalendarQueue<ITEM>::EnQueue(ITEM* item)
{
  
  if(m_head==NULL)
  {
    m_head=item;
    return;
  }
  if(m_head->time>item->time)
  {
    enqueue(m_head);
    m_head=item;
  }
  else
    enqueue(item);
}
template <class ITEM>
void CalendarQueue<ITEM>::enqueue(ITEM* item)
{
  long i;
  if(item->time<last_priority)
  {
    i=(long)(item->time/bucket_width);
    last_priority=item->time;
    bucket_top=bucket_width*(i+1)+bucket_width*0.5;
    i=i%number_of_buckets;
    last_bucket=i;
  }
  else
  {
    i=(long)(item->time/bucket_width);
    i=i%number_of_buckets;
  }

        
  

  if(buckets[i]==NULL||item->time<buckets[i]->time)
  {
    item->next=buckets[i];
    buckets[i]=item;
  }
  else
  {

    ITEM* pos=buckets[i];
    while(pos->next!=NULL&&item->time>pos->next->time)
    {
      pos=pos->next;
    }
    item->next=pos->next;
    pos->next=item;
  }
  total_number++;
  if(resizable&&total_number>top_threshold)
    ReSize(number_of_buckets*2);
}
template <class ITEM>
void CalendarQueue<ITEM>::Delete(ITEM* item)
{
  if(item==m_head)
  {
    m_head=dequeue();
    return;
  }
  long j;
  j=(long)(item->time/bucket_width);
  j=j%number_of_buckets;
        
  

  
  

  ITEM** p = &buckets[j];
  
  ITEM* i=buckets[j];
    
  while(i!=NULL)
  {
    if(i==item)
    { 
      (*p)=item->next;
      total_number--;
      if(resizable&&total_number<bottom_threshold)
	ReSize(number_of_buckets/2);
      return;
    }
    p=&(i->next);
    i=i->next;
  }   
}
template <class ITEM>
double CalendarQueue<ITEM>::NewWidth()
{
  long i, nsamples;
        
  if(total_number<2) return 1.0;
  if(total_number<=5)
    nsamples=total_number;
  else
    nsamples=5+total_number/10;
  if(nsamples>CQ_MAX_SAMPLES) nsamples=CQ_MAX_SAMPLES;
        
  long _last_bucket=last_bucket;
  double _bucket_top=bucket_top;
  double _last_priority=last_priority;
        
  double AVG[CQ_MAX_SAMPLES],avg1=0,avg2=0;
  ITEM* list,*next,*item;
        
  list=dequeue(); 
  long real_samples=0;
  while(real_samples<nsamples)
  {
    item=dequeue();
    if(item==NULL)
    {
      item=list;
      while(item!=NULL)
      {
	next=item->next;
	enqueue(item);
	item=next;      
      }

      last_bucket=_last_bucket;
      bucket_top=_bucket_top;
      last_priority=_last_priority;

                        
      return 1.0;
    }
    AVG[real_samples]=item->time-list->time;
    avg1+=AVG[real_samples];
    if(AVG[real_samples]!=0.0)
      real_samples++;
    item->next=list;
    list=item;
  }
  item=list;
  while(item!=NULL)
  {
    next=item->next;
    enqueue(item);
    item=next;      
  }
        
  last_bucket=_last_bucket;
  bucket_top=_bucket_top;
  last_priority=_last_priority;
        
  avg1=avg1/(double)(real_samples-1);
  avg1=avg1*2.0;
        
  
  long count=0;
  for(i=0;i<real_samples-1;i++)
  {
    if(AVG[i]<avg1&&AVG[i]!=0)
    {
      avg2+=AVG[i];
      count++;
    }
  }
  if(count==0||avg2==0)   return 1.0;
        
  avg2 /= (double) count;
  avg2 *= 3.0;
        
  return avg2;
}

#endif /*PRIORITY_QUEUE_H*/

#line 38 "./COST/cost.h"


#line 1 "./COST/corsa_alloc.h"
































#ifndef corsa_allocator_h
#define corsa_allocator_h

#include <typeinfo>
#include <string>

class CorsaAllocator
{
private:
    struct DT{
#ifdef CORSA_DEBUG
	DT* self;
#endif
	DT* next;
    };
public:
    CorsaAllocator(unsigned int );         
    CorsaAllocator(unsigned int, int);     
    ~CorsaAllocator();		
    void *alloc();		
    void free(void*);
    unsigned int datasize() 
    {
#ifdef CORSA_DEBUG
	return m_datasize-sizeof(DT*);
#else
	return m_datasize; 
#endif
    }
    int size() { return m_size; }
    int capacity() { return m_capacity; }			
    
    const char* GetName() { return m_name.c_str(); }
    void SetName( const char* name) { m_name=name; } 

private:
    CorsaAllocator(const CorsaAllocator& ) {}  
    void Setup(unsigned int,int); 
    void InitSegment(int);
  
    unsigned int m_datasize;
    char** m_segments;	          
    int m_segment_number;         
    int m_segment_max;      
    int m_segment_size;	          
				  
    DT* m_free_list; 
    int m_size;
    int m_capacity;

    int m_free_times,m_alloc_times;
    int m_max_allocs;

    std::string m_name;
};
#ifndef CORSA_NODEF
CorsaAllocator::CorsaAllocator(unsigned int datasize)
{
    Setup(datasize,256);	  
}

CorsaAllocator::CorsaAllocator(unsigned int datasize, int segsize)
{
    Setup(datasize,segsize);
}

CorsaAllocator::~CorsaAllocator()
{
    #ifdef CORSA_DEBUG
    printf("%s -- alloc: %d, free: %d, max: %d\n",GetName(),
	   m_alloc_times,m_free_times,m_max_allocs);
    #endif

    for(int i=0;i<m_segment_number;i++)
	delete[] m_segments[i];	   
    delete[] m_segments;			
}

void CorsaAllocator::Setup(unsigned int datasize,int seg_size)
{

    char buffer[50];
    sprintf(buffer,"%s[%d]",typeid(*this).name(),datasize);
    m_name = buffer;

#ifdef CORSA_DEBUG
    datasize+=sizeof(DT*);  
#endif

    if(datasize<sizeof(DT))datasize=sizeof(DT);
    m_datasize=datasize;
    if(seg_size<16)seg_size=16;    
    m_segment_size=seg_size;			
    m_segment_number=1;		   
    m_segment_max=seg_size;	   
    m_segments= new char* [ m_segment_max ] ;   
    m_segments[0]= new char [m_segment_size*m_datasize];  

    m_size=0;
    m_capacity=0;
    InitSegment(0);

    m_free_times=m_alloc_times=m_max_allocs=00;
}

void CorsaAllocator::InitSegment(int s)
{
    char* p=m_segments[s];
    m_free_list=reinterpret_cast<DT*>(p);
    for(int i=0;i<m_segment_size-1;i++,p+=m_datasize)
    {
	reinterpret_cast<DT*>(p)->next=
	    reinterpret_cast<DT*>(p+m_datasize);
    }
    reinterpret_cast<DT*>(p)->next=NULL;
    m_capacity+=m_segment_size;
}

void* CorsaAllocator::alloc()
{
    #ifdef CORSA_DEBUG
    m_alloc_times++;
    if(m_alloc_times-m_free_times>m_max_allocs)
	m_max_allocs=m_alloc_times-m_free_times;
    #endif
    if(m_free_list==NULL)	
    
    {
	int i;
	if(m_segment_number==m_segment_max)	
	
	
	{
	    m_segment_max*=2;		
	    char** buff;
	    buff=new char* [m_segment_max];   
#ifdef CORSA_DEBUG
	    if(buff==NULL)
	    {
		printf("CorsaAllocator runs out of memeory.\n");
		exit(1);
	    }
#endif
	    for(i=0;i<m_segment_number;i++)
		buff[i]=m_segments[i];	
	    delete [] m_segments;		
	    m_segments=buff;
	}
	m_segment_size*=2;
	m_segments[m_segment_number]=new char[m_segment_size*m_datasize];
#ifdef CORSA_DEBUG
	    if(m_segments[m_segment_number]==NULL)
	    {
		printf("CorsaAllocator runs out of memeory.\n");
		exit(1);
	    }
#endif
	InitSegment(m_segment_number);
	m_segment_number++;
    }

    DT* item=m_free_list;		
    m_free_list=m_free_list->next;
    m_size++;

#ifdef CORSA_DEBUG
    item->self=item;
    char* p=reinterpret_cast<char*>(item);
    p+=sizeof(DT*);
    
    return static_cast<void*>(p);
#else
    return static_cast<void*>(item);
#endif
}

void CorsaAllocator::free(void* data)
{
#ifdef CORSA_DEBUG
    m_free_times++;
    char* p=static_cast<char*>(data);
    p-=sizeof(DT*);
    DT* item=reinterpret_cast<DT*>(p);
    
    if(item!=item->self)
    {
	if(item->self==(DT*)0xabcd1234)
	    printf("%s: packet at %p has already been released\n",GetName(),p+sizeof(DT*)); 
	else
	    printf("%s: %p is probably not a pointer to a packet\n",GetName(),p+sizeof(DT*));
    }
    assert(item==item->self);
    item->self=(DT*)0xabcd1234;
#else
    DT* item=static_cast<DT*>(data);
#endif

    item->next=m_free_list;
    m_free_list=item;
    m_size--;
}
#endif /* CORSA_NODEF */

#endif /* corsa_allocator_h */

#line 39 "./COST/cost.h"


class trigger_t {};
typedef double simtime_t;

#ifdef COST_DEBUG
#define Printf(x) Print x
#else
#define Printf(x)
#endif



class TimerBase;



struct CostEvent
{
  double time;
  CostEvent* next;
  union {
    CostEvent* prev;
    int pos;  
  };
  TimerBase* object;
  int index;
  unsigned char active;
};



class TimerBase
{
 public:
  virtual void activate(CostEvent*) = 0;
  inline virtual ~TimerBase() {}	
};

class TypeII;



class CostSimEng
{
 public:

  class seed_t
      {
       public:
	void operator = (long seed) { srand48(seed); };
      };
  seed_t		Seed;
  CostSimEng()
      : stopTime( 0), clearStatsTime( 0), m_clock( 0.0)
      {
        if( m_instance == NULL)
	  m_instance = this;
        else
	  printf("Error: only one simulation engine can be created\n");
      }
  virtual		~CostSimEng()	{ }
  static CostSimEng	*Instance()
      {
        if(m_instance==NULL)
        {
	  printf("Error: a simulation engine has not been initialized\n");
	  m_instance = new CostSimEng;
        }
        return m_instance;
      }
  CorsaAllocator	*GetAllocator(unsigned int datasize)
      {
    	for(unsigned int i=0;i<m_allocators.size();i++)
    	{
	  if(m_allocators[i]->datasize()==datasize)return m_allocators[i];
    	} 
    	CorsaAllocator* allocator=new CorsaAllocator(datasize);
    	char buffer[25];
    	sprintf(buffer,"EventAllocator[%d]",datasize);
    	allocator->SetName(buffer);
    	m_allocators.push_back(allocator);
    	return allocator;
      }
  void		AddComponent(TypeII*c)
      {
        m_components.push_back(c);
      }
  void		ScheduleEvent(CostEvent*e)
      {
	if( e->time < m_clock)
	{
	  printf("scheduled event-> time: %f, object: %p, m_clock: %f\n",e->time,e->object,m_clock);
	  assert(e->time>=m_clock);
	}
	m_queue.EnQueue(e);
      }
  void		CancelEvent(CostEvent*e)
      {
        
        m_queue.Delete(e);
      }
  double	Random( double v=1.0)	{ return v*drand48();}
  int		Random( int v)		{ return (int)(v*drand48()); }
  double	Exponential(double mean)	{ return -mean*log(Random());}
  virtual void	Start()		{}
  virtual void	Stop()		{}
  void		Run();
  double	SimTime()	{ return m_clock; } 
  void		StopTime( double t)	{ stopTime = t; }
  double	StopTime() const	{ return stopTime; }
  void		ClearStatsTime( double t)	{ clearStatsTime = t; }
  double	ClearStatsTime() const	{ return clearStatsTime; }
  virtual void	ClearStats()	{}
 private:
  double	stopTime;
  double	clearStatsTime;	
  double	eventRate;
  double	runningTime;
  long		eventsProcessed;
  double	m_clock;
  queue_t<CostEvent>	m_queue;
  std::vector<TypeII*>	m_components;
  static CostSimEng	*m_instance;
  std::vector<CorsaAllocator*>	m_allocators;
};




class TypeII
{
 public: 
  virtual void Start() {};
  virtual void Stop() {};
  inline virtual ~TypeII() {}		
  TypeII()
      {
        m_simeng=CostSimEng::Instance();
        m_simeng->AddComponent(this);
      }

#ifdef COST_DEBUG
  void Print(const bool, const char*, ...);
#endif
    
  double Random(double v=1.0) { return v*drand48();}
  int Random(int v) { return (int)(v*drand48());}
  double Exponential(double mean) { return -mean*log(Random());}
  inline double SimTime() const { return m_simeng->SimTime(); }
  inline double StopTime() const { return m_simeng->StopTime(); }
 private:
  CostSimEng* m_simeng;
}; 

#ifdef COST_DEBUG
void TypeII::Print(const bool flag, const char* format, ...)
{
  if(flag==false) return;
  va_list ap;
  va_start(ap, format);
  printf("[%.10f] ",SimTime());
  vprintf(format,ap);
  va_end(ap);
}
#endif

CostSimEng* CostSimEng::m_instance = NULL;

void CostSimEng::Run()
{
  double	nextTime = (clearStatsTime != 0.0 && clearStatsTime < stopTime) ? clearStatsTime : stopTime;

  m_clock = 0.0;
  eventsProcessed = 0l;
  std::vector<TypeII*>::iterator iter;
      
  struct timeval start_time;    
  gettimeofday( &start_time, NULL);

  Start();

  for( iter = m_components.begin(); iter != m_components.end(); iter++)
    (*iter)->Start();

  CostEvent* e=m_queue.DeQueue();
  while( e != NULL)
  {
    if( e->time >= nextTime)
    {
      if( nextTime == stopTime)
	break;
      
      printf( "Clearing statistics @ %f\n", nextTime);
      nextTime = stopTime;
      ClearStats();
    }
    
    assert( e->time >= m_clock);
    m_clock = e->time;
    e->object->activate( e);
    eventsProcessed++;
    e = m_queue.DeQueue();
  }
  m_clock = stopTime;
  for(iter = m_components.begin(); iter != m_components.end(); iter++)
    (*iter)->Stop();
	    
  Stop();

  struct timeval stop_time;    
  gettimeofday(&stop_time,NULL);

  runningTime = stop_time.tv_sec - start_time.tv_sec +
      (stop_time.tv_usec - start_time.tv_usec) / 1000000.0;
  eventRate = eventsProcessed/runningTime;
  
  
  printf("# -------------------------------------------------------------------------\n");	
  printf("# CostSimEng with %s, stopped at %f\n", m_queue.GetName(), stopTime);	
  printf("# %ld events processed in %.3f seconds, event processing rate: %.0f\n",	
  eventsProcessed, runningTime, eventRate);
  
}







#line 8 "Sim_SlottedCSMA.cc"


#include <deque>


#line 1 "Channel.h"




#define DATARATE 11E6 // Data Transmission Rate
#define PHYRATE 1E6








#define SLOT 9e-06 
#define DIFS 28e-06
#define SIFS 10e-06
#define LDBPS 256
#define TSYM 4e-06
#define ECA_AIFS_TIME 28e-06
#define SIG_EXT 6e-06
			

#line 1 "Aux.h"
#ifndef _AUX_
#define _AUX_

struct Packet
{
	int source;
	int destination;
	int L;
	int seq;
	double tx_time; 
	double queuing_time; 
	double contention_time;
	int fairShare; 
	int aggregation; 
	int accessCategory; 
	int startContentionStage;
};

struct SLOT_notification
{
	int status; 			
	signed long long num;	
	int error;				
};


#endif 


#line 23 "Channel.h"



#line 85 "Channel.h"
;


#line 126 "Channel.h"
;


#line 139 "Channel.h"
;


#line 12 "Sim_SlottedCSMA.cc"


#line 1 "STA.h"
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <string>
#include <array>
#include <map>
#include <bitset>

#line 1 "Aux.h"
#ifndef _AUX_
#define _AUX_

struct Packet
{
	int source;
	int destination;
	int L;
	int seq;
	double tx_time; 
	double queuing_time; 
	double contention_time;
	int fairShare; 
	int aggregation; 
	int accessCategory; 
	int startContentionStage;
};

struct SLOT_notification
{
	int status; 			
	signed long long num;	
	int error;				
};


#endif 


#line 11 "STA.h"


#line 1 "FIFO.h"
#ifndef _FIFO_QUEUE_
#define _FIFO_QUEUE_



#include <deque>






template <class DATATYPE> class FIFO 
{	
	private:
		std::deque <DATATYPE> m_queue;
		
	public:
		DATATYPE GetFirstPacket();
		void DelFirstPacket();		
		void PutPacket(DATATYPE &packet);	
		int QueueSize();
};

template <class DATATYPE> DATATYPE FIFO <DATATYPE> :: GetFirstPacket()
{
	return(m_queue.front());	
}; 

template <class DATATYPE> void FIFO <DATATYPE> :: DelFirstPacket()
{
	if(m_queue.size() == 0) printf("Attempting to erase an empty queue\n");
	m_queue.pop_front();
}; 

template <class DATATYPE> void FIFO <DATATYPE> :: PutPacket(DATATYPE &packet)
{	
	m_queue.push_back(packet);
}; 

template <class DATATYPE> int FIFO <DATATYPE> :: QueueSize()
{
	return(m_queue.size());
}; 



#endif

#line 12 "STA.h"


#line 1 "includes/computeBackoff.hh"
#define AC 4

using namespace std;

void computeBackoff(int &backlog, FIFO <Packet> &Queue, int &ac, int &stickiness, int &backoffStage, 
	double &counter, int &system_stickiness, int &id, int &sx, int &ECA, double &AIFS, const int defaultAIFS[AC]){

	
	int CWmin [4] = { 16, 32, 16, 8 };

	
	
	

	if(backlog == 1)
	{
		
		if(sx == 1)
		{
			if(ECA == 1)
			{
				counter = (int)((pow(2,backoffStage)*CWmin[ac]/2)-1);

				
			}else if(ECA == 3)
			{
				counter = (int)((pow(2,0)*CWmin[ac]/2)-1);
			}

			else
			{
				counter = rand() % (int)  (pow(2,backoffStage) * CWmin[ac] );
				
			}
		}else
		{
			if(stickiness > 0)
			{
				counter = (int)((pow(2,backoffStage)*CWmin[ac]/2)-1);
				
			}else
			{
				counter = rand() % (int) (pow(2,backoffStage) * CWmin[ac] );
				
			}
			
		}
	}else
	{
		backoffStage = 0;
		counter = 0;
		stickiness = system_stickiness;
		
	}
	AIFS = defaultAIFS[ac];
}
#line 13 "STA.h"


#line 1 "includes/computeBackoff_enhanced.hh"

#line 1 "includes/concatenate.hh"
#define AC 4

using namespace std;

unsigned concatenate(unsigned x, unsigned y) {
    unsigned pow = 10;
    while(y >= pow)
        pow *= 10;
    return x * pow + y;        
}
#line 1 "includes/computeBackoff_enhanced.hh"


#line 1 "includes/isThisNewBackoffPossible_copy.hh"
#define AC 4
using namespace std;

int isThisNewBackoffPossible_copy(double test, std::array<int,AC> &stages, std::array<double,AC> &counters, int &category,
	std::array<int,AC> &backlog, int CWmin[AC]){
	
	

	int isItPossible = 0;
	std::array<int, AC> futureCycles;
	std::array<int, AC> compareBackoffs;
	std::array<int, AC> compareCycles; 
	std::array<int, AC> match;
	std::map<double,double>::iterator it;

	match.fill(0);
	futureCycles.fill(1);
	compareBackoffs.fill(1);
	compareCycles.fill(1);

	int deterministicBackoff = (int) (pow(2,(stages.at(category))) * CWmin[category]/2 - 1);

	
	for(int i = 0; i < AC; i++)
	{
		
		if(i != category)
		{
			if(backlog.at(i) == 1)
			{
				int difference = fabs( counters.at(i) - test );
				int othersDetBackoff = (pow(2,stages.at(i)) * CWmin[i]/2);
				int minimum = std::min( (int)othersDetBackoff, (int)deterministicBackoff+1 );
				futureCycles.at(i) = difference % minimum; 
			}else
			{
				futureCycles.at(i) = 1;
			}
		}
	}

	
	for(int i = 0; i < AC; i++)
	{
		if(test == counters.at(i))
		{
			compareBackoffs.at(i) = 1;	
		}else
		{
			compareBackoffs.at(i) = 0;
		}

		if(futureCycles.at(i) == 0)
		{
			compareCycles.at(i) = 1;
		}else
		{
			compareCycles.at(i) = 0;
		}
	}


	if ( (compareBackoffs == match) && (compareCycles == match) )
	{
		isItPossible = 1;
	}else
	{
		isItPossible = 0;
	}

	return(isItPossible);

}

#line 2 "includes/computeBackoff_enhanced.hh"

#define AC 4

using namespace std;

void computeBackoff_enhanced(std::array<int,AC> &backlog, FIFO <Packet> &Queue, int &category, int &stickiness, std::array<int,AC> &stages, 
	std::array<double,AC> &counters, int &system_stickiness, int &id, int &sx, int &ECA, std::map<double,double> &buffer, 
	std::array<double,AC> &AIFS, const int defaultAIFS[AC]){

	
	
	int CWmin [AC] = { 16, 32, 16, 8 }; 

	double deterministicBackoff;
	double basicDetBackoff;
	int isItPossible;

	deterministicBackoff = (int) (pow(2,(stages.at(category))) * CWmin[category]/2 - 1);
	basicDetBackoff = (int) (pow(2,0) * CWmin[category]/2 - 1);
	AIFS.at(category) = defaultAIFS[category];

	
	if(ECA == 3)
	{
		isItPossible = isThisNewBackoffPossible_copy(basicDetBackoff, stages, counters, category, backlog, CWmin);	
	}else
	{
		isItPossible = isThisNewBackoffPossible_copy(deterministicBackoff, stages, counters, category, backlog, CWmin);	
	}

	
	if(backlog.at(category) == 1)
	{
		if(isItPossible == 1)
		{
			if(sx == 1)
			{
				if(ECA == 1)
				{
					counters.at(category) = deterministicBackoff;

					
					
					return; 
				}else if(ECA == 3) 
				{
					counters.at(category) = basicDetBackoff;					
				}
			}else
			{
				if(stickiness > 0)
				{
					counters.at(category) = deterministicBackoff;	
					
					
					return;
				}
			}
		}
	}else
	{
		stages.at(category) = 0;
		counters.at(category) = 0;
		stickiness = system_stickiness;
		
		return;
	}
	


	
	
	


	
	

	

	
	
	
	
	
	
	
	
	

	
	
		

		double randomBackoff;
		std::array<int, AC> futureCycles;
		std::array<int, AC> compareBackoffs;
		std::array<int, AC> compareCycles; 
		std::array<int, AC> match;
		std::map<double,double>::iterator it;

		match.fill(0);
		futureCycles.fill(1);
		compareBackoffs.fill(1);
		compareCycles.fill(1);

		while ( (compareBackoffs != match) || (compareCycles != match) )
		{
			randomBackoff = rand() % (int) ( (pow(2,stages.at(category))) * CWmin[category] - 1);

			
			for(int i = 0; i < AC; i++)
			{
				
				if(i != category)
				{
					if(backlog.at(i) == 1)
					{
						int difference = fabs( counters.at(i) - randomBackoff );
						int othersDetBackoff = (pow(2,stages.at(i)) * CWmin[i]/2);
						int minimum = std::min( (int)othersDetBackoff, (int)deterministicBackoff+1 );
						futureCycles.at(i) = difference % minimum; 
					}else
					{
						futureCycles.at(i) = 1;
					}
				}
			}

			
			for(int i = 0; i < AC; i++)
			{
				if(randomBackoff == counters.at(i))
				{
					compareBackoffs.at(i) = 1;	
				}else
				{
					compareBackoffs.at(i) = 0;
				}

				if(futureCycles.at(i) == 0)
				{
					compareCycles.at(i) = 1;
				}else
				{
					compareCycles.at(i) = 0;
				}

			}
			
			
			
			
			
			
			
			
			
			
			
		}
		
		counters.at(category) = randomBackoff;
		
		

		
		
		
		
		
		
		


	
	
	
	
	
}
#line 14 "STA.h"


#line 1 "includes/selectMACProtocol.hh"
using namespace std;



void selectMACProtocol(const int node_id, int &ECA, int stickiness, int cut){

	

	switch(cut)
	{
		case 0:
			ECA = 1;
			if(stickiness == 0) ECA = 3;
			break;
		case 1:
			ECA = 0;
			break;
		default:
			if(node_id < cut)
			{	
				ECA = 1;
			}else
			{
				ECA = 0;
			}
	}
	
	
	
	
	
	
	
	
	
	

	
	
	
	
	
	
	
	
	
}
#line 15 "STA.h"


#line 1 "includes/resolveInternalCollision.hh"
#define AC 4

int resolveInternalCollision(std::array<double,AC> &counters, std::array<int,AC> &backlogg, std::array<int,AC> &stickiness, 
	std::array<int,AC> &stages, std::array<int,AC> &recomputeBackoff, std::array<double,AC> &totalInternalACCol,
	std::array<int,AC> &retAttemptAC, int scheme, int id, const int MAXSTAGE_ECA[AC], const int MAXSTAGE_EDCA[AC], 
	std::array<double,AC> &consecutiveSx, double timer){

	int iterator = counters.size() - 1;
	int acToTx;
	int winner = -1;

	recomputeBackoff.fill(0);

	
	
	
	
	

	for(int i = iterator; i >= 0; i--)
	{
		if(backlogg.at(i) == 1)
		{ 
			if(counters.at(i) == 0) 
			{
				winner = i; 
				break;
			}
		}
	}

	

	acToTx = winner;


	if(winner >= 0)
	{
		for(int i = 0; i < winner; i++)
		{
			if(backlogg.at(i) == 1)
			{
				if(counters.at(i) == 0)
				{
					int recompute = i;
					recomputeBackoff.at(recompute) = 1;
					totalInternalACCol.at(recompute)++;
					
					consecutiveSx.at(recompute) = 0;

					
					
					
					if(scheme == 0) 
					{
						stickiness.at(recompute) = std::max((int) stickiness.at(recompute) - 1, 0);
						stages.at(recompute) = std::min((int)stages.at(recompute) + 1, MAXSTAGE_EDCA[recompute]);

						
						
						retAttemptAC.at(recompute)++;
					}
				}
			}
		}
	}

	return(acToTx);
	
}
#line 16 "STA.h"


#line 1 "includes/preparePacketForTransmission.hh"
#include <math.h>
#include <algorithm>

#define AC 4

Packet preparePacketForTransmission(int acToTx, double txTime, std::array<Packet,AC> &superPacket, 
	int id, std::array<int,AC> &stages, std::array<FIFO <Packet>, AC> &Queues, int fairShare)
{
	superPacket.at(acToTx).source = id;
	superPacket.at(acToTx).tx_time = txTime;
	superPacket.at(acToTx).accessCategory = acToTx;

	if(fairShare == 1)
	{		
		superPacket.at(acToTx).aggregation = std::min( (int)pow(2,stages.at(acToTx)), 
			Queues.at(acToTx).QueueSize() );
	}

	return((Packet)(superPacket.at(acToTx)));
}
#line 17 "STA.h"


#line 1 "includes/erasePacketsFromQueue.hh"

#line 1 "includes/../Aux.h"
#ifndef _AUX_
#define _AUX_

struct Packet
{
	int source;
	int destination;
	int L;
	int seq;
	double tx_time; 
	double queuing_time; 
	double contention_time;
	int fairShare; 
	int aggregation; 
	int accessCategory; 
	int startContentionStage;
};

struct SLOT_notification
{
	int status; 			
	signed long long num;	
	int error;				
};


#endif 


#line 1 "includes/erasePacketsFromQueue.hh"


using namespace std;

void erasePacketsFromQueue(std::array<FIFO <Packet>, AC> &Queues, Packet &packet, int id, 
    int &backlogged, int fairShare, int sx, double &dropped, std::array<double,AC> &qEmpty, int &affected)
{
    int packetDisposal = 0;
    int aggregation = (int)packet.aggregation;
    int cat = (int)packet.accessCategory;

    if(cat >= 0)
    {
        if(sx == 1)
        {
            packetDisposal = std::min( (aggregation - affected), (int)Queues.at(cat).QueueSize() );

            
            
            
            
            
            
        }else
        {
            if(fairShare == 1)
            {
                packetDisposal = std::min( (int)pow(2, packet.startContentionStage), 
                    (int)Queues.at(cat).QueueSize() );
            }else
            {
                packetDisposal = std::min( aggregation, (int)Queues.at(cat).QueueSize() );
            }

            
            
            
            
            
            

            dropped+= packetDisposal;
        }
        
        
        for(int i = 0; i < packetDisposal; i++) Queues.at(cat).DelFirstPacket();
        


        if (Queues.at(cat).QueueSize() > 0)
        {   
            backlogged = 1;
        }else
        {
            backlogged = 0;
            qEmpty.at(cat)++;
        }
    }

}

#line 18 "STA.h"


#line 1 "includes/pickNewPacket.hh"

#line 1 "includes/../Aux.h"
#ifndef _AUX_
#define _AUX_

struct Packet
{
	int source;
	int destination;
	int L;
	int seq;
	double tx_time; 
	double queuing_time; 
	double contention_time;
	int fairShare; 
	int aggregation; 
	int accessCategory; 
	int startContentionStage;
};

struct SLOT_notification
{
	int status; 			
	signed long long num;	
	int error;				
};


#endif 


#line 1 "includes/pickNewPacket.hh"

#define AC 4

using namespace std;

void pickNewPacket(int &accessCategory, double pickupTime, std::array<Packet,AC> &superPacket, 
	std::array<FIFO <Packet>, AC> &Queues, int id, std::array<int,AC> stages, int fairShare, int maxAggregation, 
	const int MAXSTAGE_EDCA[AC], const int MAXSTAGE_ECA[AC], int ECA)

{
	Packet packet = Queues.at(accessCategory).GetFirstPacket();
	packet.contention_time = pickupTime;
	superPacket.at(accessCategory).contention_time = packet.contention_time;
	superPacket.at(accessCategory).startContentionStage = stages.at(accessCategory);
	superPacket.at(accessCategory).fairShare = fairShare; 
	
	int packets = 1;
	if(fairShare == 1)
	{
		packets = std::min( (int)pow(2,stages.at(accessCategory)), Queues.at(accessCategory).QueueSize() );
	}

	if(maxAggregation == 1)
	{
		int exponent = MAXSTAGE_EDCA[accessCategory];
		if(ECA == 1) exponent = MAXSTAGE_ECA[accessCategory];
		packets = std::min( (int)pow(2,exponent), Queues.at(accessCategory).QueueSize() );
	}

	superPacket.at(accessCategory).aggregation = packets;
}

#line 19 "STA.h"


#line 1 "includes/decrement.hh"
#define AC 4

using namespace std;

void decrement(int category, double &counter, double &AIFS, int station, double timer)
{
	if(AIFS == 0)
	{
		counter --;
	}else
	{
		AIFS --;
	}

	
}
#line 20 "STA.h"


#line 1 "includes/setAIFS.hh"
#define AC 4

using namespace std;
void setAIFS(std::array<double,AC> &AIFS, int ECA, const int defaultAIFS[AC], const int ECA_AIFS[AC] )
{
	if(ECA == 0)	
	{	
		for(int i = 0; i < AC; i++)
		{
			AIFS.at(i) = defaultAIFS[i];
		}
	}else
	{
		for(int i = 0; i < AC; i++)
		{
			AIFS.at(i) = ECA_AIFS[i];
		}
	}
}
#line 21 "STA.h"


#line 1 "includes/analiseHalvingCycle.hh"
#define AC 4

using namespace std;

void analiseHalvingCycle(std::array<double,AC> &consecutiveSx, std::array<double,AC> &halvingCounters,
	std::array<int,AC> &stages, std::array<double,AC> &counters, int acToTx, const int MAXSTAGES[AC],
	std::array<int,AC> backlog, std::array<int,AC> &halvingAttempt, int slot, std::array<int, AC> &shouldHalve,
	std::array<int,AC> &halvingThresholds, int node, std::array<int,AC> &changeStage, std::array<double,AC> &halved,
	std::array<int, AC> &stationStickiness, int systemStickiness){

	int CWmin [AC] = { 32, 32, 16, 8 }; 


	
	for(int i = 0; i < AC; i++)
	{
		if(halvingAttempt.at(i) > 0)
		{
			if(halvingCounters.at(i) == 0)
			{
				shouldHalve.at(i) += slot;
				halvingCounters.at(i) = -1;	
				if(shouldHalve.at(i) > 0)
				{
					halvingAttempt.at(i) = 0;	
				}

				
			}else if(halvingCounters.at(i) > 0)
			{
				halvingCounters.at(i)--;	
			}
			
			
			
		}
	}

	
	for(int i = 0; i < AC; i++)
	{
		if( (halvingCounters.at(i) == -1) && ( (int)halvingAttempt.at(i) == (int)halvingThresholds.at(i) ) ) 
		{
			if(shouldHalve.at(i) == 0) 
			{
				
				
				
				
				

				changeStage.at(i) = 1;
				halvingAttempt.at(i) = 0;
			}else
			{
				
				
				
				
				changeStage.at(i) = 0;
			}
		}
	}

	
	for(int i = 0; i < AC; i++)
	{
		if(backlog.at(i) == 1)
		{
			if(counters.at(i) == 0)
			{	
				if(changeStage.at(i) == 1) 
				{
					stages.at(i) = std::max(0, stages.at(i) - 1);
					stationStickiness.at(i) = std::min(stationStickiness.at(i)+1, systemStickiness+1);	
					consecutiveSx.at(i) = 0;
					changeStage.at(i) = 0;
					halved.at(i)++;

					
					
					

				}
				
				

				
				halvingThresholds.at(i) = ( (int)(pow(2, (MAXSTAGES[i]-1) ) * CWmin[i] / 2) / (int)(pow(2, stages.at(i)) * CWmin[i] / 2) ) -1;

				
				

				if(halvingThresholds.at(i) == 0) halvingThresholds.at(i) = 1;

				
					
					

				if( ((int)consecutiveSx.at(i) >= (int)halvingThresholds.at(i)) ) 
				{
					int previousStage = std::max(0, stages.at(i) - 1);
					halvingAttempt.at(i)++;
					halvingCounters.at(i) = (pow(2,previousStage) * CWmin[i]/2 + 1); 	
																						
					
					
					
				}
			}
		}
	}
}
#line 22 "STA.h"


#line 1 "includes/analiseResetCycle.hh"
#define AC 4

#line 1 "includes/isThisNewBackoffPossible.hh"
#define AC 4
using namespace std;

int isThisNewBackoffPossible(double test, std::array<int,AC> &stages, std::array<double,AC> &counters, int &category,
	std::array<int,AC> &backlog, int CWmin[AC]){
	
	

	int isItPossible = 0;
	std::array<int, AC> futureCycles;
	std::array<int, AC> compareBackoffs;
	std::array<int, AC> compareCycles; 
	std::array<int, AC> match;
	std::map<double,double>::iterator it;

	match.fill(0);
	futureCycles.fill(1);
	compareBackoffs.fill(1);
	compareCycles.fill(1);

	int deterministicBackoff = (int) (pow(2,(stages.at(category))) * CWmin[category]/2 - 1);

	
	for(int i = 0; i < AC; i++)
	{
		
		if(i != category)
		{
			if(backlog.at(i) == 1)
			{
				int difference = fabs( counters.at(i) - test );
				int othersDetBackoff = (pow(2,stages.at(i)) * CWmin[i]/2);
				int minimum = std::min( (int)othersDetBackoff, (int)deterministicBackoff+1 );
				futureCycles.at(i) = difference % minimum; 
			}else
			{
				futureCycles.at(i) = 1;
			}
		}
	}

	
	for(int i = 0; i < AC; i++)
	{
		if(test == counters.at(i))
		{
			compareBackoffs.at(i) = 1;	
		}else
		{
			compareBackoffs.at(i) = 0;
		}

		if(futureCycles.at(i) == 0)
		{
			compareCycles.at(i) = 1;
		}else
		{
			compareCycles.at(i) = 0;
		}
	}


	if ( (compareBackoffs == match) && (compareCycles == match) )
	{
		isItPossible = 1;
	}else
	{
		isItPossible = 0;
	}

	
	

	return(isItPossible);

}
#line 2 "includes/analiseResetCycle.hh"

using namespace std;

void analiseResetCycle(std::array<double,AC> &consecutiveSx, std::array<double,AC> &resetCounters,
	std::array<int,AC> &stages, std::array<double,AC> &counters, int acToTx, const int MAXSTAGES[AC],
	std::array<int,AC> backlog, std::array<int,AC> &resetAttempt, SLOT_notification slot, std::array<int, AC> &shouldReset,
	std::array<int,AC> &resetThresholds, int node, std::array<int,AC> &changeStage, std::array<double,AC> &reset,
	std::array<int, AC> &stationStickiness, int systemStickiness, std::array<double,AC> &analysisCounter, double timer){

	int CWmin [AC] = { 32, 32, 16, 8 }; 
	int newStage = 0;


	for(int i = 0; i < AC; i++)
	{
		
		if(resetAttempt.at(i) == 1)
		{
			
			if(resetCounters.at(i) >= 0)
			{
				
				if(slot.status > 0)
				{
					shouldReset.at(i) += pow(2, (int)( analysisCounter.at(i) - resetCounters.at(i) ) );
					
					
				}
				resetCounters.at(i)--;	
			}
		}
	}

	
	for(int i = 0; i < AC; i++)
	{
		
		if(resetCounters.at(i) == -1)
		{
			
			std::bitset< 512 > scheduleMap ( (int) shouldReset.at(i) );
			
			
			for(int j = 0; j < stages.at(i); j++)
			{
				int increments = (pow(2,j) * CWmin[i]/2);
				int slotAvailable = 0;
				changeStage.at(i) = 0; 
				
				
				

				
				
				
				
				
				
				
				
				


				
				
				for(int k = increments-1; k <= analysisCounter.at(i); k += increments)
				{
					slotAvailable += scheduleMap[k];
					
					
				}
				
				

				
				if(slotAvailable == 0)
				{
					changeStage.at(i) = 1;
					newStage = j;
					break;
				}else
				{
					
					
					
				}
			}
		}
	}

	
	for(int i = 0; i < AC; i++)
	{
		if(backlog.at(i) == 1)
		{
			if(counters.at(i) == 0)
			{	
				
				if(changeStage.at(i) == 1)
				{
					double newDeterministicBackoff = (int) (pow(2,newStage) * CWmin[i]/2 - 1);
					int canWeChange = isThisNewBackoffPossible(newDeterministicBackoff, stages, counters, i, backlog, CWmin);
					if(canWeChange == 1)
					{
						
						stationStickiness.at(i) = std::min(stationStickiness.at(i)+1, systemStickiness+1);
						
						resetAttempt.at(i) = 0;
						resetCounters.at(i) = 0;
						consecutiveSx.at(i) = 0;
						changeStage.at(i) = 0;
						reset.at(i)++;
						stages.at(i) = newStage;
						
						
						
						
					}else
					{
						
						
					}

				}
				if(stages.at(i) != 0)
				{
					
					resetThresholds.at(i) = ( (int)(pow(2, MAXSTAGES[i]) * CWmin[i] / 2) ) / (int)( pow(2, stages.at(i)) * CWmin[i] / 2 - 1 );

					
					

					
					

					if(resetThresholds.at(i) <= 0) resetThresholds.at(i) = 1;

					
						
						

					
					if( ((int)consecutiveSx.at(i) >= (int)resetThresholds.at(i)) )
					{
						resetAttempt.at(i) = 1;
						
						resetCounters.at(i) = (pow(2,stages.at(i)) * CWmin[i]/2) -1;
						
						analysisCounter.at(i) = resetCounters.at(i);	
						
						
						
						
					}
				}
			}
		}
	}
}
#line 23 "STA.h"


#line 1 "includes/analiseBetterReset.hh"
#define AC 4

using namespace std;

void analiseBetterReset(std::array<double,AC> &consecutiveSx, std::array<double,AC> &resetCounters,
	std::array<int,AC> &stages, std::array<double,AC> &counters, int acToTx, const int MAXSTAGES[AC],
	std::array<int,AC> backlog, std::array<int,AC> &resetAttempt, SLOT_notification slot, std::array<int, AC> &shouldReset,
	std::array<int,AC> &resetThresholds, int node, std::array<int,AC> &changeStage, std::array<double,AC> &reset,
	std::array<int, AC> &stationStickiness, int systemStickiness, std::array<double,AC> &analysisCounter, 
	double timer, std::bitset< 512 > &scheduleMap, std::array<int,AC> &resetSx){

	int CWmin [AC] = { 16, 32, 16, 8 }; 
	int newStage = 0;
	int halving = 1;	
	int dynamicStickiness = 1;	
	int conservativeApproach = 0;	


	for(int i = 0; i < AC; i++)
	{
		
		if(resetAttempt.at(i) == 1)
		{
			
			if(resetCounters.at(i) > 0)
			{
				
				resetCounters.at(i)--;	
				
				if(slot.status > 0)
				{
					
					shouldReset.at(i) += pow(2, (int)( analysisCounter.at(i) - (resetCounters.at(i) + 1) ) );
					
					
				}
				

			}
		}
	}

	
	for(int i = 0; i < AC; i++)
	{
		
		if(resetAttempt.at(i) == 1)
		{
			
			if(resetCounters.at(i) == 0)
			{
				
				std::bitset< 512 > pastSchedule ( (int) shouldReset.at(i) );
				
				shouldReset.at(i) = 0;

				
				
				
				
				
				
				scheduleMap |= pastSchedule;
				

				
				pastSchedule.reset();

				
				int bitmapIteration = (int)resetThresholds.at(i);
				if(conservativeApproach == 1) bitmapIteration = (int)(pow(2,MAXSTAGES[i]) * CWmin[i] / 2) / 
					(pow(2,stages.at(i)) * CWmin[i] / 2);

				if(consecutiveSx.at(i) >= bitmapIteration)
				{
					
					
					for(int j = 0; j < stages.at(i); j++)
					{
						int increments = (pow(2,j) * CWmin[i]/2);
						if(halving == 1) increments = (pow(2,max(0,stages.at(i)-1)) * CWmin[i]/2);
						int slotAvailable = 0;
						changeStage.at(i) = 0; 

						
						
						
						
						
						
						
						
						
						
						
						


						
						
						for(int k = increments; k < analysisCounter.at(i); k += increments)
						{
							slotAvailable += scheduleMap[k];
							
							
						}
						
						

						


						
						if(slotAvailable == 0)
						{
							changeStage.at(i) = 1;
							newStage = j;
							if(halving == 1) newStage = stages.at(i) -1;
							break;
						}else
						{
							
							
							
						}
					}
				}
			}
		}
	}

	
	for(int i = 0; i < AC; i++)
	{
		if(backlog.at(i) == 1)
		{
			if(counters.at(i) == 0)
			{	
				
				if(changeStage.at(i) == 1)
				{
					double newDeterministicBackoff = (int) (pow(2,newStage) * CWmin[i]/2 - 1);
					int canWeChange = isThisNewBackoffPossible(newDeterministicBackoff, stages, counters, i, backlog, CWmin);
					if(canWeChange == 1)
					{
						
						if(dynamicStickiness == 1) stationStickiness.at(i) = systemStickiness+1;
						
						resetAttempt.at(i) = 0;
						resetCounters.at(i) = 0;
						consecutiveSx.at(i) = 0;
						changeStage.at(i) = 0;
						reset.at(i)++;
						resetSx.at(i) = 1;
						
						
						
						
						stages.at(i) = newStage;
						scheduleMap.reset();
						
					}else
					{
						
						
					}

				}

				
				
				if(stages.at(i) >= 0)
				{
					
					

					
					

					
					resetThresholds.at(i) = 1;

					if(resetThresholds.at(i) <= 0) resetThresholds.at(i) = 1;

					
						
						

					
					if( ((int)consecutiveSx.at(i) >= (int)resetThresholds.at(i)) )
					{
						resetAttempt.at(i) = 1;
						
						resetCounters.at(i) = (pow(2,stages.at(i)) * CWmin[i]/2);
						
						analysisCounter.at(i) = resetCounters.at(i);	
						
						
						
						
					}
				}
			}
		}
	}
}
#line 24 "STA.h"





#define MAX_RET 7


#define AC 4


extern "C" const int MAXSTAGE_ECA [AC] = { 5, 5, 5, 5 };
extern "C" const int MAXSTAGE_EDCA [AC] = { 5, 5, 1, 1 };
extern "C" const int ECA_AIFS [AC] = { 0, 0, 0, 0 };
extern "C" const int defaultAIFS [AC] = { 0, 0, 0, 0 };



using namespace std;


#line 153 "STA.h"
;


#line 206 "STA.h"
;


#line 307 "STA.h"
;


#line 690 "STA.h"
;


#line 13 "Sim_SlottedCSMA.cc"



#line 1 "BatchPoissonSource_enhanced.h"



			

#line 1 "Aux.h"
#ifndef _AUX_
#define _AUX_

struct Packet
{
	int source;
	int destination;
	int L;
	int seq;
	double tx_time; 
	double queuing_time; 
	double contention_time;
	int fairShare; 
	int aggregation; 
	int accessCategory; 
	int startContentionStage;
};

struct SLOT_notification
{
	int status; 			
	signed long long num;	
	int error;				
};


#endif 


#line 5 "BatchPoissonSource_enhanced.h"


#define MAXSEQ 1024
#define AC 4


#line 50 "BatchPoissonSource_enhanced.h"
;


#line 56 "BatchPoissonSource_enhanced.h"
;
	

#line 61 "BatchPoissonSource_enhanced.h"
;


#line 120 "BatchPoissonSource_enhanced.h"
;


#line 15 "Sim_SlottedCSMA.cc"


#line 1 "stats/stats.h"
#include <math.h>
#include <iostream>

using namespace std;

double stats(int successful_slots, int empty_slots, int collision_slots, int payload){
    
    double Ts, Tc;
    double throughput;
    
    
    const double PHY_RATE = 1E6;
    const double RBASIC = 1E6;
    const double RDATA = 11E6;
    const double PCLP_PREAMBLE = 144; 
    const double PCLP_HEADER = 48; 
    
    const double MAC_HEADER = 272; 
    const double L_ACK = 112; 
    
    
    const double SLOT_TIME = 20E-6;
    const double DIFS2 = 50E-6;
    const double SIFS2 = 10E-6;
    const double EIFS2 = SIFS2 + DIFS2 + ((144 + 48 + 112)/RBASIC);

    
    const double L = payload*8;
    
    
    
    Ts = ((PCLP_PREAMBLE + PCLP_HEADER)/PHY_RATE) + ((L + MAC_HEADER)/RDATA) + SIFS2 + ((PCLP_PREAMBLE + PCLP_HEADER)/PHY_RATE) + (L_ACK/RBASIC) + DIFS2;
    Tc = ((PCLP_PREAMBLE + PCLP_HEADER)/RBASIC)+ ((L+MAC_HEADER)/RDATA) + EIFS2;
    
    
    
    
    throughput = successful_slots*L/(empty_slots*SLOT_TIME+successful_slots*Ts+collision_slots*Tc);
    
    return(throughput);
}

#line 16 "Sim_SlottedCSMA.cc"




using namespace std;


#line 166 "Sim_SlottedCSMA.cc"
;


#line 171 "Sim_SlottedCSMA.cc"
;


#line 646 "Sim_SlottedCSMA.cc"
;

#include "compcxx_Sim_SlottedCSMA.h"
class compcxx_Channel_6;/*template <class T> */
#line 269 "./COST/cost.h"
class compcxx_Timer_3 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_3() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
  inline void Set(trigger_t const &, double );
  inline void Set(double );
  inline double GetTime() { return m_event.time; }
  inline bool Active() { return m_event.active; }
  inline trigger_t & GetData() { return m_event.data; }
  inline void SetData(trigger_t const &d) { m_event.data = d; }
  void Cancel();
  /*outport void to_component(trigger_t &)*/;
  void activate(CostEvent*);
 private:
  CostSimEng* m_simeng;
  event_t m_event;
public:compcxx_Channel_6* p_compcxx_parent;};

class compcxx_Channel_6;/*template <class T> */
#line 269 "./COST/cost.h"
class compcxx_Timer_2 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_2() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
  inline void Set(trigger_t const &, double );
  inline void Set(double );
  inline double GetTime() { return m_event.time; }
  inline bool Active() { return m_event.active; }
  inline trigger_t & GetData() { return m_event.data; }
  inline void SetData(trigger_t const &d) { m_event.data = d; }
  void Cancel();
  /*outport void to_component(trigger_t &)*/;
  void activate(CostEvent*);
 private:
  CostSimEng* m_simeng;
  event_t m_event;
public:compcxx_Channel_6* p_compcxx_parent;};

class compcxx_Channel_6;/*template <class T> */
#line 269 "./COST/cost.h"
class compcxx_Timer_4 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_4() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
  inline void Set(trigger_t const &, double );
  inline void Set(double );
  inline double GetTime() { return m_event.time; }
  inline bool Active() { return m_event.active; }
  inline trigger_t & GetData() { return m_event.data; }
  inline void SetData(trigger_t const &d) { m_event.data = d; }
  void Cancel();
  /*outport void to_component(trigger_t &)*/;
  void activate(CostEvent*);
 private:
  CostSimEng* m_simeng;
  event_t m_event;
public:compcxx_Channel_6* p_compcxx_parent;};


#line 25 "Channel.h"
class compcxx_Channel_6 : public compcxx_component, public TypeII
{

	public:
		void Setup();
		void Start();
		void Stop();
			
	public:
		int Nodes;
		float error;

		
		class my_Channel_out_slot_f_t:public compcxx_functor<Channel_out_slot_f_t>{ public:void  operator() (SLOT_notification &slot) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(slot); return (c[0]->*f[0])(slot);};};compcxx_array<my_Channel_out_slot_f_t > out_slot;/*outport void out_slot(SLOT_notification &slot)*/;	
		/*inport */void in_packet(Packet &packet);

		
		compcxx_Timer_2 /*<trigger_t> */slot_time; 
		compcxx_Timer_3 /*<trigger_t> */rx_time; 
		compcxx_Timer_4 /*<trigger_t> */triggerChange; 
		
		/*inport */inline void NewSlot(trigger_t& t1);
		/*inport */inline void EndReceptionTime(trigger_t& t2);
		/*inport */inline void changeConditions(trigger_t& t3);

		compcxx_Channel_6 () { 
			slot_time.p_compcxx_parent=this /*connect slot_time.to_component,*/; 
			rx_time.p_compcxx_parent=this /*connect rx_time.to_component,*/;
		    triggerChange.p_compcxx_parent=this /*connect triggerChange.to_component,*/; 
		}

	private:
		double number_of_transmissions_in_current_slot;
		int affected;	
		double succ_tx_duration, collision_duration; 
		double empty_slot_duration;
		double TBack;
		double L_max;
		int MAC_H, PLCP_PREAMBLE, PLCP_HEADER;
		int aggregation;
		float errorProbability;
		int rate48;	
		
		
     	ofstream slotsInTime;

	public: 
		double collision_slots, empty_slots, succesful_slots, error_slots, total_slots;
		double totalBitsSent;
		double recentCollisions; 
		double pastRecentCollisions;
		double recentErrors;
		int errorPeriod;
		bool channelModel;	
		signed long long slotNum;
};

class compcxx_BatchPoissonSource_8;/*template <class T> */
#line 269 "./COST/cost.h"
class compcxx_Timer_5 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_5() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
  inline void Set(trigger_t const &, double );
  inline void Set(double );
  inline double GetTime() { return m_event.time; }
  inline bool Active() { return m_event.active; }
  inline trigger_t & GetData() { return m_event.data; }
  inline void SetData(trigger_t const &d) { m_event.data = d; }
  void Cancel();
  /*outport void to_component(trigger_t &)*/;
  void activate(CostEvent*);
 private:
  CostSimEng* m_simeng;
  event_t m_event;
public:compcxx_BatchPoissonSource_8* p_compcxx_parent;};


#line 10 "BatchPoissonSource_enhanced.h"
class compcxx_BatchPoissonSource_8 : public compcxx_component, public TypeII
{
	public:
		
		class my_BatchPoissonSource_out_f_t:public compcxx_functor<BatchPoissonSource_out_f_t>{ public:void  operator() (Packet &packet) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(packet); return (c[0]->*f[0])(packet);};};my_BatchPoissonSource_out_f_t out_f;/*outport void out(Packet &packet)*/;	

		
		compcxx_Timer_5 /*<trigger_t> */inter_packet_timer;
		
		/*inport */inline void new_packet(trigger_t& t);

		compcxx_BatchPoissonSource_8 () { 
			inter_packet_timer.p_compcxx_parent=this /*connect inter_packet_timer.to_component,*/;
		}

	public:
		int L;
		long int seq;
		int MaxBatch;	
		double packet_rate;
		int categories;
		int packetGeneration;
		double packetsGenerated;
		std::array<double,AC> packetsInAC;

		int BEShare;
		int BKShare;
		int VIShare;
		int VOShare;
	
	public:
		void Setup();
		void Start();
		void Stop();
			
};


#line 44 "STA.h"
class compcxx_STA_7 : public compcxx_component, public TypeII
{
    public:
        void Setup();
        void Start();
        void Stop();

    public:
    	
        int node_id;
        int K; 
        int system_stickiness; 
        std::array<int, AC> stationStickiness; 
        int fairShare;
        
        
        int cut;
        int ECA;
        int backoffScheme;

        
        std::map<double,double> buffer;
        
        
        int maxAggregation;
        int L; 
        
        
        double incommingPackets;
        std::array<double,AC> blockedPackets;
        double totalBloked;
        std::array<double,AC> queuesSizes;
        double overallPacketsInQueue;
        std::array<double,AC> packetsInQueue;
        std::array<double,AC> queueEmpties;
        double erased;
        double remaining;


        
        std::array<double,AC> transmissions;
        std::array<double,AC> sxTx;
        std::array<double,AC> consecutiveSx;
        std::array<double,AC> packetsSent; 
        double overallSentPackets;
        std::array<double,AC> overallACThroughput;
        double overallThroughput;
        int saturated;
        std::bitset< 512 > scheduleMap;

        
        double totalCollisions;
        std::array<double,AC> totalACCollisions;

        double totalInternalCol;
        std::array<double,AC> totalInternalACCol;
        std::array<int,AC> recomputeBackoff;

        double totalRetransmissions;
        std::array<double,AC> totalACRet;
        std::array<int,AC> retAttemptAC;
        double totalDropped;
        std::array<double,AC> droppedAC;

        
        std::array<double,AC> accumTimeBetweenSxTx;

    private:
    	

        std::array<double, AC> backoffCounters;
        std::array<int, AC> backoffStages;
        std::array<double,AC> AIFS;

        
        int changingSchedule; 
        std::array<double, AC> halvingCounters;
        std::array<double, AC> analysisCounter;
        std::array<int,AC> halvingThresholds;
        std::array<int, AC> halvingAttempt;
        std::array<int, AC> shouldHalve; 
        std::array<int,AC> changeStage;
        std::array<double,AC> halved; 
        std::array<int,AC> resetSuccessfull; 

        
        int transmitted;    
        int sx;             

        int ACToTx; 
        
        std::array<int,AC> backlogged; 

		Packet packet; 
        std::array<Packet,AC> superPacket; 
        
        std::array<FIFO <Packet>, AC> Queues;
        FIFO <Packet> Q;

    public:
        
        /*inport */void inline in_slot(SLOT_notification &slot);
        /*inport */void inline in_packet(Packet &packet);
        class my_STA_out_packet_f_t:public compcxx_functor<STA_out_packet_f_t>{ public:void  operator() (Packet &packet) { for (unsigned int compcxx_i=1;compcxx_i<c.size();compcxx_i++)(c[compcxx_i]->*f[compcxx_i])(packet); return (c[0]->*f[0])(packet);};};my_STA_out_packet_f_t out_packet_f;/*outport void out_packet(Packet &packet)*/;
};


#line 22 "Sim_SlottedCSMA.cc"
class compcxx_SlottedCSMA_9 : public compcxx_component, public CostSimEng
{
	public:
		void Setup(int Sim_Id, int NumNodes, int PacketLength, double Bandwidth, int Batch, int Stickiness, int ECA, int fairShare, float channelErrors, float slotDrift,float percentageDCF, int maxAggregation, int howManyACs, int simSeed);
		void Stop();
		void Start();		

	public:
		compcxx_Channel_6 channel;

		compcxx_array<compcxx_STA_7  >stas;
		compcxx_array<compcxx_BatchPoissonSource_8  >sources;

	private:
		int SimId;
		int Nodes;
		double Bandwidth_;
		int PacketLength_;
		double percentageEDCA_;
		int Batch_;
		float drift;
		double intCut, decimalCut, cut; 
		

};


#line 290 "./COST/cost.h"

#line 290 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_3/*<trigger_t >*/::Set(trigger_t const & data, double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.data = data;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 302 "./COST/cost.h"

#line 302 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_3/*<trigger_t >*/::Set(double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 313 "./COST/cost.h"

#line 313 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_3/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 321 "./COST/cost.h"

#line 321 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_3/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->EndReceptionTime(m_event.data));
}




#line 290 "./COST/cost.h"

#line 290 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_2/*<trigger_t >*/::Set(trigger_t const & data, double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.data = data;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 302 "./COST/cost.h"

#line 302 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_2/*<trigger_t >*/::Set(double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 313 "./COST/cost.h"

#line 313 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_2/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 321 "./COST/cost.h"

#line 321 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_2/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->NewSlot(m_event.data));
}




#line 290 "./COST/cost.h"

#line 290 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_4/*<trigger_t >*/::Set(trigger_t const & data, double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.data = data;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 302 "./COST/cost.h"

#line 302 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_4/*<trigger_t >*/::Set(double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 313 "./COST/cost.h"

#line 313 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_4/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 321 "./COST/cost.h"

#line 321 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_4/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->changeConditions(m_event.data));
}




#line 46 "Channel.h"

#line 47 "Channel.h"

#line 48 "Channel.h"

#line 82 "Channel.h"
void compcxx_Channel_6 :: Setup()
{

}
#line 87 "Channel.h"
void compcxx_Channel_6 :: Start()
{
	number_of_transmissions_in_current_slot = 0;
	affected = 0;
	succ_tx_duration = 0;
	collision_duration = 0;
	empty_slot_duration = 9e-06;

	collision_slots = 0;
	empty_slots = 0;
	succesful_slots = 0;
	total_slots = 0;
	error_slots = 0;
	recentCollisions = 0;
	pastRecentCollisions = 0;
	recentErrors = 0;
	slotNum = 0;
	errorPeriod = 0;
	channelModel = true;
	triggerChange.Set(SimTime());

	L_max = 0;
	
	MAC_H = 272;
	PLCP_PREAMBLE = 144; 
	PLCP_HEADER = 48;
	
	TBack = 32e-06 + ceil((16 + 256 + 6)/LDBPS) * TSYM;
	totalBitsSent = 0;

	aggregation = 0;
	errorProbability = 0;

	rate48 = 1;

	slot_time.Set(SimTime()); 
	
	slotsInTime.open("Results/slotsInTime.txt", ios::app);

}
#line 128 "Channel.h"
void compcxx_Channel_6 :: Stop()
{
	printf("\n\n");
	printf("---- Channel ----\n");
	printf("Slot Status Probabilities (channel point of view): Empty = %e, Succesful = %e, Collision = %e \n",empty_slots/total_slots,succesful_slots/total_slots,collision_slots/total_slots);
	printf("Total packets sent to the Channel: %d\n", (int)succesful_slots);
	printf("\n\n");


	
	slotsInTime.close();
}
#line 141 "Channel.h"
void compcxx_Channel_6 :: changeConditions(trigger_t &)
{
	if(error > 0 && errorPeriod > 0)
	{
		channelModel = !channelModel;
		
		
		triggerChange.Set(SimTime() + errorPeriod);
	}

}


#line 153 "Channel.h"
void compcxx_Channel_6 :: NewSlot(trigger_t &)
{
	

	SLOT_notification slot;
	slotNum++;

	slot.status = number_of_transmissions_in_current_slot;
	slot.num = slotNum;
	slot.error = affected;

	number_of_transmissions_in_current_slot = 0;
	affected = 0;
	L_max = 0;

	for(int n = 0; n < Nodes; n++) out_slot[n](slot); 

	rx_time.Set(SimTime());	
}


#line 173 "Channel.h"
void compcxx_Channel_6 :: EndReceptionTime(trigger_t &)
{
    


	if(number_of_transmissions_in_current_slot==0) 
	{
		slot_time.Set(SimTime()+SLOT);
		empty_slots++;
	}
	if(number_of_transmissions_in_current_slot == 1)
	{
		slot_time.Set(SimTime()+succ_tx_duration);
		if(affected > 0)
		{
			error_slots++;
			recentErrors++;
			totalBitsSent += (aggregation - affected)*(L_max*8);

		}else
		{
			totalBitsSent += aggregation*(L_max*8);
			succesful_slots++;
		}
	}
	if(number_of_transmissions_in_current_slot > 1)
	{
		collision_slots++;	
		recentCollisions++;
		slot_time.Set(SimTime()+collision_duration);
	}
		
	total_slots++; 
	
	
	
	int increment = 100;
    if(int(total_slots) % increment == 0) 
	{
	        slotsInTime << Nodes << " " << SimTime() << " " <<  total_slots << " " << collision_slots/total_slots << " " 
	        	<< succesful_slots/total_slots << " " << empty_slots/total_slots << " " << recentCollisions << " " 
	        	<< error_slots/total_slots << " " << recentErrors << endl;

	        if(pastRecentCollisions == 0)
	        {
	        	pastRecentCollisions = collision_slots;	
	        }else
	        {
	        	if(pastRecentCollisions == collision_slots)
	        	{
	        		
	        	}else
	        	{
	        		pastRecentCollisions = collision_slots;
	        	}
	        }

        	recentCollisions = 0;
        	recentErrors = 0;
	}
	
}



#line 237 "Channel.h"
void compcxx_Channel_6 :: in_packet(Packet &packet)
{

	if(packet.L > L_max) L_max = packet.L;
	
	aggregation = packet.aggregation;

	int model = channelModel;
	switch(model)
	{

		case 0:
			number_of_transmissions_in_current_slot++;
			break;

		case 1:
			for(int i = 1; i <= aggregation; i++)
			{
			    
			    
				
				errorProbability = rand() % (int) 100;
				if( (errorProbability > 0) && (errorProbability <= (int)(error*100)) )
				{
					affected++;
				}		
			}
			if(affected > 0)
			{
				if(affected == aggregation)
				{
					number_of_transmissions_in_current_slot++; 
				}	
			}
		    number_of_transmissions_in_current_slot++;
			break;
		default:
			number_of_transmissions_in_current_slot++;
			break;
	}

	
	
		
		
		
		
			
		
			succ_tx_duration = (SIFS + 32e-06 + ceil((16 + aggregation*(32+(L_max*8)+288) + 6)/LDBPS)*TSYM + SIFS + TBack + DIFS + empty_slot_duration);
	
			if(rate48 == 1)
			{
				
				

				
				
				
				double ACK = 20.0 + ceil(134.0/192.0) * 4.0 + 6.0;

				
				
				double frame = 20.0 + ceil((L_max*8.0 + 534.0)/192.0) * 4.0 + 6.0;

				
				succ_tx_duration = frame * 1e-06 + SIFS + ACK * 1e-06 + DIFS + 9.0 * 7.5 * 1e-06;

				
			}

	
	collision_duration = succ_tx_duration;
	
	

	
	
}


#line 290 "./COST/cost.h"

#line 290 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_5/*<trigger_t >*/::Set(trigger_t const & data, double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.data = data;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 302 "./COST/cost.h"

#line 302 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_5/*<trigger_t >*/::Set(double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 313 "./COST/cost.h"

#line 313 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_5/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 321 "./COST/cost.h"

#line 321 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_5/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->new_packet(m_event.data));
}




#line 19 "BatchPoissonSource_enhanced.h"

#line 47 "BatchPoissonSource_enhanced.h"
void compcxx_BatchPoissonSource_8 :: Setup()
{

}
#line 52 "BatchPoissonSource_enhanced.h"
void compcxx_BatchPoissonSource_8 :: Start()
{
	if(packet_rate > 0) inter_packet_timer.Set(Exponential(1/packet_rate));
	seq = 0;
}
#line 58 "BatchPoissonSource_enhanced.h"
void compcxx_BatchPoissonSource_8 :: Stop()
{

}
#line 63 "BatchPoissonSource_enhanced.h"
void compcxx_BatchPoissonSource_8 :: new_packet(trigger_t &)
{
	packetGeneration = rand() % (int) (100);
	Packet packet;

	switch(categories)
	{
		case 1:
			packet.accessCategory = 0;
			break;
		case 2:
			if( (packetGeneration >= VIShare) && (packetGeneration < BEShare) )
			{
				packet.accessCategory = 1;
			}else
			{
				packet.accessCategory = 0;
			}
			break;
		case 3:
			if( (packetGeneration >= VOShare) && (packetGeneration < VIShare) )
			{
				packet.accessCategory = 2;
			}else if( (packetGeneration >= VIShare) && (packetGeneration < BEShare) )
			{
				packet.accessCategory = 1;
			}else
			{
				packet.accessCategory = 0;
			}
			break;
		default:
			if(packetGeneration < VOShare)
			{
				packet.accessCategory = 3;
			}else if( (packetGeneration >= VOShare) && (packetGeneration < VIShare) )
			{
				packet.accessCategory = 2;
			}else if( (packetGeneration >= VIShare) && (packetGeneration < BEShare) )
			{
				packet.accessCategory = 1;
			}else
			{
				packet.accessCategory = 0;
			}
			break;
	}

	int RB = (int) Random(MaxBatch)+1;
	
	packet.L = L;
	packet.queuing_time = SimTime();
	(out_f(packet));
	packetsGenerated += 1;
	packetsInAC.at(packet.accessCategory) += 1;

	inter_packet_timer.Set(SimTime()+Exponential(RB/packet_rate));	
}
#line 150 "STA.h"
void compcxx_STA_7 :: Setup()
{

}
#line 155 "STA.h"
void compcxx_STA_7 :: Start()
{
	selectMACProtocol(node_id, ECA, system_stickiness, cut);
    setAIFS(AIFS, ECA, defaultAIFS, ECA_AIFS);

    

    backoffScheme = 1; 
    changingSchedule = 0; 
    if(ECA == 0){
        backoffScheme = 0;
        changingSchedule = 0; 
    }

    
	
	

	incommingPackets = 0;

    for(int i = 0; i < (int)superPacket.size(); i++)
    {
        superPacket.at(i).contention_time = SimTime();
        
        superPacket.at(i).aggregation = 1;
        superPacket.at(i).L = L;
    }

    ACToTx = -1;
    transmitted = 0;
    sx = 0;

    for(int i = 0; i < AC; i++)
    {
        stationStickiness.at(i) = system_stickiness; 
        droppedAC.at(i) = 0;
        backlogged.at(i) = 0;
        backoffStages.at(i) = 0;
        Queues.at(i) = Q;
        overallACThroughput.at(i) = 0.0;
        consecutiveSx.at(i) = 0;
        halvingCounters.at(i) = 0.0;
        analysisCounter.at(i) = 0.0;
        halvingThresholds.at(i) = 1;
        halvingAttempt.at(i) = 0;
        shouldHalve.at(i) = 0;
        changeStage.at(i) = 0;
        halved.at(i) = 0;
        resetSuccessfull.at(i) = 0;
    }
	
}
#line 208 "STA.h"
void compcxx_STA_7 :: Stop()
{

    cout << "\n***Node-" << node_id << "**" <<endl;
    for(int it = 0; it < AC; it++)
    {
        cout << "AC " << it << endl;
        cout << "\t* Final Stickiness: " << stationStickiness.at(it) << " (system's: " << system_stickiness << ")." << endl;
        cout << "\t* Total transmission attempts for AC " << it << ": " << transmissions.at(it) << endl;
        cout << "\t+ Total Packets sent for AC " << it << ": " << packetsSent.at(it) << endl;
        
        if(packetsSent.at(it) > 0){
            overallACThroughput.at(it) = (packetsSent.at(it) * L * 8.)/SimTime();
        }
        cout << "\t+ Throughput for AC " << it << ": " << overallACThroughput.at(it) << endl;
        overallSentPackets += packetsSent.at(it);

        if(sxTx.at(it) == 0) sxTx.at(it)++;   
        
        if(backlogged.at(it) == 1) accumTimeBetweenSxTx.at(it) += (double)(SimTime() - superPacket.at(it).contention_time);
        cout << "\t- Time between successful transmissions for AC " << it << ": " << 
        accumTimeBetweenSxTx.at(it) / sxTx.at(it) << endl;

        packetsInQueue.at(it) = Queues.at(it).QueueSize();
        overallPacketsInQueue += packetsInQueue.at(it);
        cout << "\t* Queue AC " << it << ": " << packetsInQueue.at(it) << endl;
        cout << "\t* Number of queue flushes for AC " << it << ": " << queueEmpties.at(it) << endl;

        totalCollisions += totalACCollisions.at(it);
        totalInternalCol += totalInternalACCol.at(it);
        cout << "\t- Total Collisions for AC " << it << ": " << totalACCollisions.at(it) << endl;
        cout << "\t\t- Total Internal Collisions for AC " << it << ": " << totalInternalACCol.at(it) << endl;

        totalRetransmissions += totalACRet.at(it);
        cout << "\t- Total Retransmissions for AC " << it << ": " << totalACRet.at(it) << endl;

        totalDropped += droppedAC.at(it);
        cout << "\t- Total Dropped due to retransmissions for AC " << it << ": " << droppedAC.at(it) << endl;

        totalBloked += blockedPackets.at(it);
        cout << "\t- Total Blocked due to full MAC queue for AC " << it << ": " << blockedPackets.at(it) << endl;

    }
    cout << "+ Overall successful transmissions: " << overallSentPackets << endl;
    
    overallThroughput = (overallSentPackets * L * 8.) / SimTime();
    cout << "+ Overall throughput for this station: " << overallThroughput << endl;

    cout << "- Overal Collisions for this station: " << totalCollisions << endl;

    cout << "- Overall retransmissions: " << totalRetransmissions << endl;

    cout << "- Overall incomming packets: " << incommingPackets << endl;

    cout << "- Overall dropped due to RET: " << totalDropped << endl;

    cout << "- Total Blocked packet due to full MAC queue: " << totalBloked << endl;

    double pFailureI_1 = 0;
    double pFailureI_2 = 0;

    erased = totalDropped + overallSentPackets;
    remaining = overallPacketsInQueue + totalBloked;

    pFailureI_1 = (incommingPackets - erased) / remaining;
    pFailureI_2 = incommingPackets / (erased + remaining);
    
    cout << "- Erased packets failure index (at least one should be 1). In sat: " <<
    pFailureI_1 << ". Non-sat: " << pFailureI_2 << endl;

    
    double totalHalved = 0.0;
    for(int i = 0; i < AC; i++)
    {
        totalHalved += halved.at(i);
    }
    cout << "+ Overall times some deterministic backoff was changed : " << totalHalved << endl;
    
    
    
    
    

    
    
    
    
    string logName = "sta-";
    string staNum = to_string(node_id);
    string end = ".log";
    logName = logName + staNum + end;
    string routeToFile = "Results/stations/";
    routeToFile = routeToFile + logName;

    ofstream staFile;
    staFile.open(routeToFile.c_str(), ios::app);
    staFile << node_id << " " << overallThroughput << " " << totalCollisions << " " << totalHalved << endl;
    staFile.close();
    
}
#line 309 "STA.h"
void compcxx_STA_7 :: in_slot(SLOT_notification &slot)
{	

    
	switch(slot.status)
	{
		case 0:
			
            
			for(int i = 0; i < AC; i++)
			{
                
                
				if(backlogged.at(i) == 1) 
				{
                    if(backoffCounters.at(i) > 0)
                    {
                        decrement(i, backoffCounters.at(i), AIFS.at(i), node_id, SimTime());
                        
                    }
				}
            }
            
            for(int i = 0; i < AC; i++)
            {
                if(backlogged.at(i) == 0)
                {
                    
                    if(Queues.at(i).QueueSize() > 0)
                    {
                        
                        backlogged.at(i) = 1;

                        pickNewPacket(i, SimTime(), superPacket, Queues, node_id, backoffStages, fairShare, 
                            maxAggregation, MAXSTAGE_EDCA, MAXSTAGE_ECA, ECA);
                        
                        

                        int forceRandom = 0;
                        sx = 0;                        
                        if(backoffScheme == 0)
                        {
                            computeBackoff(backlogged.at(i), Queues.at(i), i, forceRandom, backoffStages.at(i), 
                                backoffCounters.at(i), system_stickiness, node_id, sx, ECA, AIFS.at(i), defaultAIFS);
                        }else
                        {
                            computeBackoff_enhanced(backlogged, Queues.at(i), i, forceRandom, backoffStages, 
                                backoffCounters, system_stickiness, node_id, sx, ECA, buffer, AIFS, ECA_AIFS);
                        }
                        
                        

                    }
                }
			}
			break;
		case 1:
            if(transmitted == 1)
            {
                sx = 1; 
                
                for(int i = 0; i < AC; i++)
                {
                    if(i != packet.accessCategory){
                        
                        if(backlogged.at(i) == 1)
                        {
                            if(backoffCounters.at(i) > 0)
                            {
                                if(ECA == 0){
                                    AIFS.at(i) = defaultAIFS[i]; 
                                    
                                }
                                decrement(i, backoffCounters.at(i), AIFS.at(i), node_id, SimTime());    
                            }
                        }
                    }
                }
                
                for (int i = 0; i < AC; i++)
                {
				    if( (packet.accessCategory == i) && (backoffCounters.at(i) == 0) )
				    {
                        
                        packetsSent.at(i) += (packet.aggregation - slot.error);
                        
                        sxTx.at(i)++;
                        consecutiveSx.at(i)++;
                        if(resetSuccessfull.at(i) == 1) resetSuccessfull.at(i); 
                        accumTimeBetweenSxTx.at(i) += double(SimTime() - superPacket.at(i).contention_time);

                        
                        

                        
                        erasePacketsFromQueue(Queues, superPacket.at(i), node_id, backlogged.at(i), fairShare, 
                            sx, droppedAC.at(i), queueEmpties, slot.error);

                        
                    
                        

                        if(stationStickiness.at(i) <= system_stickiness)        
                        {
                            stationStickiness.at(i) = system_stickiness;        
                        }
                        if(ECA == 0 || system_stickiness == 0) backoffStages.at(i) = 0;                   
                        retAttemptAC.at(i) = 0;                                 
                        transmitted = 0;                                        

                        
                        if(backlogged.at(i) == 1)
                        {
                            pickNewPacket(i, SimTime(), superPacket, Queues, node_id, backoffStages, fairShare, 
                                maxAggregation, MAXSTAGE_EDCA, MAXSTAGE_ECA, ECA);
                            

                            
                            
                            if(backoffScheme == 0)
                            {
                                computeBackoff(backlogged.at(i), Queues.at(i), i, stationStickiness.at(i), backoffStages.at(i), 
                                    backoffCounters.at(i), system_stickiness, node_id, sx, ECA, AIFS.at(i), defaultAIFS);
                            }else
                            {
                                computeBackoff_enhanced(backlogged, Queues.at(i), i, stationStickiness.at(i), backoffStages, 
                                    backoffCounters, system_stickiness, node_id, sx, ECA, buffer, AIFS, ECA_AIFS);
                            }
                            
                        }else
                        {
                            backoffStages.at(i) = 0;
                            backoffCounters.at(i) = 0;
                            stationStickiness.at(i) = system_stickiness;
                        }
				    }
                }
			}else
            {
                for(int i = 0; i < AC; i++)
                {
                    
                    if(backoffCounters.at(i) > 0)
                    {
                        if(ECA == 0){
                                AIFS.at(i) = defaultAIFS[i]; 
                                
                        }
                        decrement(i, backoffCounters.at(i), AIFS.at(i), node_id, SimTime());
                    }
                }
            }
            break;

        default: 
            if (transmitted == 1)
            {
                sx = 0; 

                for(int i = 0; i < AC; i++)
                {
                    if( (packet.accessCategory != i) && (backoffCounters.at(i) > 0) )
                    {
                        if(ECA == 0){
                                AIFS.at(i) = defaultAIFS[i]; 
                                
                        }
                        decrement(i, backoffCounters.at(i), AIFS.at(i), node_id, SimTime());
                    }
                }

                for (int i = 0; i < AC; i++)
                {
                    if( (packet.accessCategory == i) && (backoffCounters.at(i) == 0) )
                    {
                        
                        totalACCollisions.at(i)++;

                        
                        
                        if(resetSuccessfull.at(i) == 1)
                        {
                                int maxStage = MAXSTAGE_ECA[i];
                                if(ECA == 0)
                                {
                                    maxStage = MAXSTAGE_EDCA[i];
                                }

                                backoffStages.at(i) = min( (backoffStages.at(i) + 1), maxStage );
                                resetSuccessfull.at(i) = 0;
                        }
                        

                        
                        totalACRet.at(i)++;
                        retAttemptAC.at(i)++;

                        if(retAttemptAC.at(i) == MAX_RET)
                        {
                            
                            

                            
                            accumTimeBetweenSxTx.at(i) += double(SimTime() - superPacket.at(i).contention_time);

                            erasePacketsFromQueue(Queues, superPacket.at(i), node_id, backlogged.at(i), 
                                fairShare, sx, droppedAC.at(i), queueEmpties, slot.error);

                            stationStickiness.at(i) = system_stickiness;

                            if(ECA == 0 || ECA == 3) backoffStages.at(i) = 0;

                            if(backlogged.at(i) == 1)
                            {
                                pickNewPacket(i, SimTime(), superPacket, Queues, node_id, backoffStages, fairShare, 
                                    maxAggregation, MAXSTAGE_EDCA, MAXSTAGE_ECA, ECA);
                            }else
                            {
                                backoffStages.at(i) = 0;
                                backoffCounters.at(i) = 0;
                                stationStickiness.at(i) = system_stickiness;
                            }

                            retAttemptAC.at(i) = 0;     

                        }else
                        {
                            stationStickiness.at(i) = max( (stationStickiness.at(i) - 1), 0 );
                            if(stationStickiness.at(i) == 0) 
                            {
                                
                                consecutiveSx.at(i) = 0;
                                halvingAttempt.at(i) = 0;
                                int maxStage = MAXSTAGE_ECA[i];
                                if(ECA == 0)
                                {
                                    maxStage = MAXSTAGE_EDCA[i];
                                }

                                backoffStages.at(i) = min( (backoffStages.at(i) + 1), maxStage );
                            }
                        }
                        
                        

                        

                        if(backoffScheme == 0)
                        {
                            computeBackoff(backlogged.at(i), Queues.at(i), i, stationStickiness.at(i), backoffStages.at(i), 
                                backoffCounters.at(i), system_stickiness, node_id, sx, ECA, AIFS.at(i), defaultAIFS);
                        }else
                        {
                            computeBackoff_enhanced(backlogged, Queues.at(i), i, stationStickiness.at(i), backoffStages, 
                                backoffCounters, system_stickiness, node_id, sx, ECA, buffer, AIFS, ECA_AIFS);
                        }

                        
                        transmitted = 0;
                    }
                }  
            }else
            {
                for(int i = 0; i < AC; i++)
                {
                    
                    if(backoffCounters.at(i) > 0)
                    {
                        if(ECA == 0){
                                AIFS.at(i) = defaultAIFS[i]; 
                                
                        }
                        decrement(i, backoffCounters.at(i), AIFS.at(i), node_id, SimTime());
                    }
                }
            }
	}
	
	
	
    
    ACToTx = resolveInternalCollision(backoffCounters, backlogged, stationStickiness, backoffStages, 
        recomputeBackoff, totalInternalACCol, retAttemptAC, backoffScheme, node_id, MAXSTAGE_ECA, MAXSTAGE_EDCA, 
        consecutiveSx, SimTime());


    
    if(ACToTx >= 0)
    {
        for(int i = 0; i < ACToTx; i++)
        {
            if(backlogged.at(i) == 1)
            {
                if(retAttemptAC.at(i) == MAX_RET)
                {
                    
                    

                    
                    accumTimeBetweenSxTx.at(i) += double(SimTime() - superPacket.at(i).contention_time);

                    erasePacketsFromQueue(Queues, superPacket.at(i), node_id, backlogged.at(i), 
                        fairShare, sx, droppedAC.at(i), queueEmpties, slot.error);

                    stationStickiness.at(i) = system_stickiness;

                    if(ECA == 0) backoffStages.at(i) = 0;

                    if(backlogged.at(i) == 1)
                    {
                        pickNewPacket(i, SimTime(), superPacket, Queues, node_id, backoffStages, fairShare, maxAggregation, 
                            MAXSTAGE_EDCA, MAXSTAGE_ECA, ECA);
                    }else
                    {
                        backoffStages.at(i) = 0;
                        backoffCounters.at(i) = 0;
                        stationStickiness.at(i) = system_stickiness;
                        recomputeBackoff.at(i) = 0; 
                    }
                }
                retAttemptAC.at(i) = 0;     
            }
        }
        
        
        
        for(int i = 0; i < ACToTx; i++)
        {
            if(recomputeBackoff.at(i) == 1)
            {
                sx = 0;
                

                
                int forceRandom = 0;
                if(backoffScheme == 0)
                {
                    computeBackoff(backlogged.at(i), Queues.at(i), i, forceRandom, backoffStages.at(i), 
                        backoffCounters.at(i), system_stickiness, node_id, sx, ECA, AIFS.at(i), defaultAIFS);
                }else
                {
                    computeBackoff_enhanced(backlogged, Queues.at(i), i, forceRandom, backoffStages, 
                        backoffCounters, system_stickiness, node_id, sx, ECA, buffer, AIFS, ECA_AIFS);
                }
            }
        }

        
        

        if(backoffCounters.at(ACToTx) == 0)
        {
            packet = preparePacketForTransmission(ACToTx, SimTime(), superPacket, node_id, backoffStages, Queues, fairShare);
            
            

            transmissions.at(ACToTx)++;
            transmitted = 1;
            (out_packet_f(packet));
        }
    }

    
    
    if( (changingSchedule == 1) && (ECA == 1) && (system_stickiness > 0) )
    {
        
        
        

        
        
        

        analiseBetterReset(consecutiveSx, halvingCounters, backoffStages, backoffCounters, ACToTx,
        MAXSTAGE_ECA, backlogged, halvingAttempt, slot, shouldHalve, halvingThresholds, node_id, 
        changeStage, halved, stationStickiness, system_stickiness, analysisCounter, SimTime(), 
        scheduleMap, resetSuccessfull);
    }
    

}
#line 692 "STA.h"
void compcxx_STA_7 :: in_packet(Packet &packet)
{
    incommingPackets++;

    if( Queues.at(packet.accessCategory).QueueSize()  < K )
    {
        Queues.at(packet.accessCategory).PutPacket(packet);
    }else
    {
        blockedPackets.at(packet.accessCategory)++;
    }
}


#line 48 "Sim_SlottedCSMA.cc"
void compcxx_SlottedCSMA_9 :: Setup(int Sim_Id, int NumNodes, int PacketLength, double Bandwidth, int Batch, int Stickiness, int ECA, int fairShare, float channelErrors, float slotDrift, float percentageEDCA, int maxAggregation, int howManyACs, int simSeed)
{
	SimId = Sim_Id;
	Nodes = NumNodes;
	drift = slotDrift;

	stas.SetSize(NumNodes);
	sources.SetSize(NumNodes);

	
	channel.Nodes = NumNodes;
	channel.out_slot.SetSize(NumNodes);
	channel.error = channelErrors;

	
	
	if(percentageEDCA == 0 || percentageEDCA == 1)
	{
		intCut = percentageEDCA;
	}else
	{
		cut = NumNodes * percentageEDCA;
		decimalCut = modf(cut, &intCut);
		
		if(decimalCut > 0.5)
		{
			intCut++;	
		}
	}
	
	for(int n=0;n<NumNodes;n++)
	{
		
		stas[n].node_id = n;
		stas[n].K = 1000;
		stas[n].system_stickiness = Stickiness;
		stas[n].ECA = ECA;
		stas[n].fairShare = fairShare;
		
		stas[n].cut = intCut;     
		stas[n].maxAggregation = maxAggregation;
		stas[n].L = PacketLength;


		
		sources[n].L = PacketLength;
		sources[n].categories = howManyACs;
		sources[n].packetsGenerated = 0;
		for(int i = 0; i < 4; i++) sources[n].packetsInAC.at(i) = 0;		

		
		
		sources[n].BKShare = 100; 
		sources[n].BEShare = 60;  
		sources[n].VIShare = 30;  
		sources[n].VOShare = 15;  
		sources[n].MaxBatch = Batch;
	}

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
		for(int i = 0; i < NumNodes; i++)
		{
			sources[i].packet_rate = Bandwidth/(PacketLength * 8);
			stas[i].saturated = 1;
			if(Bandwidth < 10e6) stas[i].saturated = 0;
			
		}
	
	
	
	
	
	
	
	
	
	for(int n=0;n<NumNodes;n++)
	{
        channel.out_slot[n].Connect(stas[n],(compcxx_component::Channel_out_slot_f_t)&compcxx_STA_7::in_slot) /*connect channel.out_slot[n],stas[n].in_slot*/;
		stas[n].out_packet_f.Connect(channel,(compcxx_component::STA_out_packet_f_t)&compcxx_Channel_6::in_packet) /*connect stas[n].out_packet,channel.in_packet*/;
		sources[n].out_f.Connect(stas[n],(compcxx_component::BatchPoissonSource_out_f_t)&compcxx_STA_7::in_packet) /*connect sources[n].out,stas[n].in_packet*/;
	}


	Bandwidth_ = Bandwidth;
	PacketLength_ = PacketLength;
	Batch_ = Batch;
	percentageEDCA_ = percentageEDCA;
		

}
#line 168 "Sim_SlottedCSMA.cc"
void compcxx_SlottedCSMA_9 :: Start()
{
	cout << ("--------------- Starting ---------------") << endl;
}
#line 173 "Sim_SlottedCSMA.cc"
void compcxx_SlottedCSMA_9 :: Stop()
{	
	
	
	

	
	array <double,AC> totalACthroughput = {};
	array <double,AC> totalACthroughputEDCA = {};
	array <double,AC> totalACthroughputECA = {};
	double totalThroughput = 0.0;
	double totalThroughputEDCA = 0.0;
	double totalThroughputECA = 0.0;
	
	array <double,AC> overallSxTx = {};
	double totalSentPackets = 0.0;

	array <double,AC> overallTx = {};
	double totalTx = 0.0;
	array <double,AC> totalACRet = {};
	double totalRetransmissions = 0.0;

	array <double,AC> totalACCol = {};
	array <double,AC> totalACColEDCA = {};
	array <double,AC> totalACColECA = {};
	double totalCol = channel.collision_slots;
	double sumOfCol = 0.0;

	array <double,AC> totalIntACCol = {};
	double totalIntCol = 0.0;

	array <double,AC> droppedAC = {};
	double totalDropped = 0.0;

	double totalIncommingPackets = 0;
	double totalErasedPackets = 0;
	double totalRemainingPackets = 0;

	array <double,AC> fairnessAC = {};
	array <double,AC> fairnessACNum = {};
	array <double,AC> fairnessACDenom = {};
	double overallFairnessNum = 0.0;
	double overallFairnessDenom = 0.0;

	array <double,AC> avgTimeBetweenACSxTx = {};
	array <double,AC> avgTimeBetweenACSxTxEDCA = {};
	array <double,AC> avgTimeBetweenACSxTxECA = {};

	array <double,AC> qEmpties = {};

	double totalSource = 0.0;
	array <double,AC> sourceForAC = {};



	for (int i = 0; i < Nodes; i++){
		for (int j = 0; j < AC; j++){
			if(stas[i].overallACThroughput.at(j) > 0)
			{
				totalThroughput += stas[i].overallACThroughput.at(j);
				totalACthroughput.at(j) += stas[i].overallACThroughput.at(j);
				if(stas[i].ECA == 0)
				{
					totalThroughputEDCA += stas[i].overallACThroughput.at(j);
					totalACthroughputEDCA.at(j) += stas[i].overallACThroughput.at(j);
				}else{
					totalThroughputECA += stas[i].overallACThroughput.at(j);
					totalACthroughputECA.at(j) += stas[i].overallACThroughput.at(j);
				}
			}

			if(stas[i].packetsSent.at(j) > 0)
			{
				totalSentPackets += (stas[i].packetsSent.at(j));
				overallSxTx.at(j) += (stas[i].packetsSent.at(j));
			}
			
			if(stas[i].transmissions.at(j) > 0)
			{
				totalTx += (stas[i].transmissions.at(j));
				overallTx.at(j) += (stas[i].transmissions.at(j));
			}

			if(stas[i].totalACRet.at(j) > 0)
			{
				totalRetransmissions += (stas[i].totalACRet.at(j));
				totalACRet.at(j) += (stas[i].totalACRet.at(j));
			}

			
			
				sumOfCol += (stas[i].totalACCollisions.at(j));
				totalACCol.at(j) += ( stas[i].totalACCollisions.at(j) / stas[i].transmissions.at(j) );
				if(stas[i].ECA == 0)
				{
					totalACColEDCA.at(j) += ( stas[i].totalACCollisions.at(j) / stas[i].transmissions.at(j) );
				}else{
					totalACColECA.at(j) += ( stas[i].totalACCollisions.at(j) / stas[i].transmissions.at(j) );
				}
			

			if(stas[i].totalInternalACCol.at(j) > 0)
			{
				totalIntCol += (stas[i].totalInternalACCol.at(j));
				totalIntACCol.at(j) += (stas[i].totalInternalACCol.at(j));
			}

			if(stas[i].droppedAC.at(j) > 0)
			{
				totalDropped += (stas[i].droppedAC.at(j));
				droppedAC.at(j) += (stas[i].droppedAC.at(j));
			}

			if(stas[i].overallACThroughput.at(j) > 0)
			{
				fairnessACNum.at(j) += (stas[i].overallACThroughput.at(j));
				overallFairnessNum += (stas[i].overallACThroughput.at(j));
				fairnessACDenom.at(j) += (double)pow(stas[i].overallACThroughput.at(j), 2);
				overallFairnessDenom += (double)pow(stas[i].overallACThroughput.at(j), 2);
			}

			if(stas[i].sxTx.at(j) > 0)
			{
				avgTimeBetweenACSxTx.at(j) += (stas[i].accumTimeBetweenSxTx.at(j) / stas[i].sxTx.at(j));
				if(stas[i].ECA == 0)
				{
					avgTimeBetweenACSxTxEDCA.at(j) += (stas[i].accumTimeBetweenSxTx.at(j) / stas[i].sxTx.at(j));
				}else
				{
					avgTimeBetweenACSxTxECA.at(j) += (stas[i].accumTimeBetweenSxTx.at(j) / stas[i].sxTx.at(j));
				}
			}
			
			qEmpties.at(j) += (stas[i].queueEmpties.at(j));

			totalSource += (sources[i].packetsInAC.at(j));
			sourceForAC.at(j) += (sources[i].packetsInAC.at(j));

		}
		totalIncommingPackets += (stas[i].incommingPackets);
		totalErasedPackets += (stas[i].erased);
		totalRemainingPackets += (stas[i].remaining);

	}

	ofstream file;
	file.open("Results/output.txt", ios::app);
	file << "#1. Nodes 					2. totalThroughput 			3. totalBKThroughput 		4. totalBEThroughput "<< endl;
	file << "#5. totalVIThroughput 		6. totalVOThroughput 		7. totalCollisionsSlots 	8. fractionBKCollisions "<< endl;
	file << "#9. fractionBECollisions	10. fractionVICollisions 	11. fractionVOCollisions 	12. totalInternalCollisions" << endl;
	file << "#13. totalBKIntCol 		14. totalBEIntCol 			15. totalVIIntCol 	 		16. totalVOIntCol" << endl;
	file << "#17. overallFairness 		18. BKFairness				19. BEFairness				20. VIFairness"	<< endl;
	file << "#21. VOFairness			22. avgTimeBtSxTxBK			23. avgTimeBtSxTxBE			24. avgTimeBtSxTxVI" << endl;
	file << "#25. avgTimeBtSxTxVO		26. qEmptyBK				27. qEmptyBE				28. qEmptyVI" << endl;
	file << "#29. qEmptyVO				30. totalDropped			31. droppedBK				32. droppedBE" << endl;
	file << "#33. droppedVI				34. droppedVO				35. channelErrors           36. Stickiness" << endl;
	file << "#37. totalThroughputUnsat	38. totalThroughputSat		39. totalThroughputEDCA		40. EDCABKthroughput" << endl;
	file << "#41. EDCABEthroughput		42. EDCAVIthroughput		43. EDCAVOthroughput		44.totalThroughputECA" << endl;
	file << "#45. ECABKthroughput 		46. ECABEthroughput 		47. ECAVIthroughput 		48. ECAVOthroughput" << endl;
	file << "#49 avgTimeBtSxTxBK-EDCA	50. avgTimeBtSxTxBE-EDCA 	51. avgTimeBtSxTxVI-EDCA	52. avgTimeBtSxTxVO-EDCA" << endl;
	file << "#53 avgTimeBtSxTxBK-ECA 	54. avgTimeBtSxTxBE-ECA 	55. avgTimeBtSxTxVI-ECA 	56. avgTimeBtSxTxVO-ECA" << endl;
	file << "#57. fractBKCollisionsEDCA	58. fractBECollisionsEDCA 	59. fractVICollisionsEDCA 	60. fractVOCollisionsEDCA" << endl;
	file << "#61. fractBKCollisionsECA 	62. fractBECollisionsECA 	63. fractVICollisionsECA 	64. fractVOCollisionsECA" << endl;
	
	file << Nodes << " " << totalThroughput << " ";
	
	
	for (int i = 0; i < AC; i++){
		file << totalACthroughput.at(i) << " ";
	}

	
	file << totalCol << " ";
	for (int i = 0; i < AC; i++){
		if(totalACCol.at(i) > 0)
		{
			file << (double)(totalACCol.at(i)/Nodes) << " ";
		}else
		{
			file << "0 ";
		}
	}

	
	file << totalIntCol << " ";
	for (int i = 0; i < AC; i++){
		file << totalIntACCol.at(i) << " ";
	}

	
	file << (double) ( (pow(overallFairnessNum,2)) / (Nodes * (overallFairnessDenom)) ) << " ";
	for (int i = 0; i < AC; i++){
		if(fairnessACDenom.at(i) > 0) 
		{
			fairnessAC.at(i) = (double) ( (pow(fairnessACNum.at(i),2)) / (Nodes * (fairnessACDenom.at(i))) );
		}else
		{
			fairnessAC.at(i) = 0.0;
		}
		
		file << fairnessAC.at(i) << " ";
	}

	
	for(int i = 0; i < AC; i++){
		if(avgTimeBetweenACSxTx.at(i) != 0)
		{
			file << (double)avgTimeBetweenACSxTx.at(i)/Nodes << " ";
		}else
		{
			file << "0 ";
			avgTimeBetweenACSxTx.at(i) = 0.0;
		}
	}

	
	for(int i = 0; i < AC; i++){
		if(qEmpties.at(i) != 0)
		{
			file << qEmpties.at(i) << " ";
		}else
		{
			file << "0 ";
			qEmpties.at(i) = 0;
		}
	}

	
	if(totalDropped > 0)
	{
		file << totalDropped << " ";
	}else
	{
		file << "0 ";
		totalDropped = 0;
	}
	for(int i = 0; i < AC; i++)
	{
		if(droppedAC.at(i) > 0)
		{
			file << droppedAC.at(i) << " ";
		}else
		{
			file << "0 ";
			droppedAC.at(i) = 0;
		}
	}

	
	file << channel.error << " ";

	
	file << stas[0].system_stickiness << " ";

	
	double totalThroughputUnsat = 0.0;
	double totalThroughputSat = 0.0;
	for(int i = 0; i < Nodes; i++)
	{
		if(stas[i].saturated == 0)
		{		
			totalThroughputUnsat += stas[i].overallThroughput;
		}else
		{
			totalThroughputSat += stas[i].overallThroughput;		
		}
	}
	file << totalThroughputUnsat << " " << totalThroughputSat << " ";

	
	file << totalThroughputEDCA << " ";

	for (int i = 0; i < AC; i++){
		file << totalACthroughputEDCA.at(i) << " ";
	}

	
	file << totalThroughputECA << " ";
	for (int i = 0; i < AC; i++){
		file << totalACthroughputECA.at(i) << " ";
	}

	
	int EDCAnodes = 0;
	int ECAnodes = 0;
	for(int n = 0; n < Nodes; n++)
	{
		if(stas[n].ECA == 0)
		{
			EDCAnodes++;
		}else
		{
			ECAnodes++;
		}
	}
	for(int i = 0; i < AC; i++){
		if(avgTimeBetweenACSxTxEDCA.at(i) != 0)
		{
			file << (double)avgTimeBetweenACSxTxEDCA.at(i)/EDCAnodes << " ";
		}else
		{
			file << "0 ";
			avgTimeBetweenACSxTxEDCA.at(i) = 0.0;
		}
	}

	
	for(int i = 0; i < AC; i++){
		if(avgTimeBetweenACSxTxECA.at(i) != 0)
		{
			file << (double)avgTimeBetweenACSxTxECA.at(i)/ECAnodes << " ";
		}else
		{
			file << "0 ";
			avgTimeBetweenACSxTxECA.at(i) = 0.0;
		}
	}

	
	for (int i = 0; i < AC; i++){
		if(totalACColEDCA.at(i) > 0)
		{
			file << (double)(totalACColEDCA.at(i)/EDCAnodes) << " ";
		}else
		{
			file << "0 ";
		}
	}

	
	for (int i = 0; i < AC; i++){
		if(totalACColECA.at(i) != 0)
		{
			file << (double)(totalACColECA.at(i)/ECAnodes) << " ";
		}
	}

	file << endl;


	
	
	

	cout << endl;
	cout << "---------------------------"<< endl;
	cout << "--- Overall Statistics ----" << endl;
	cout << "---------------------------"<< endl;

	double count = channel.succesful_slots  + channel.collision_slots + channel.error_slots + channel.empty_slots;
	if(count == channel.total_slots)
	{
	cout << "0. Channel Statistics: total slots " << channel.total_slots << ", sx slots " << channel.succesful_slots <<
		", colision slots " << channel.collision_slots << ", channel error slots " << channel.error_slots << ", empty slots "
		<< channel.empty_slots << endl << endl;
	}

	cout << "1. Total transmissions: " << totalTx << ". Total Throughput (Mbps): " << totalThroughput << endl;
	cout << "1.1 Packet generation rate: " << Bandwidth_ << endl;
	for(int i = 0; i < AC; i++)
	{
		cout << "\tAC " << i << ": " << overallTx.at(i) << ". Total Throughput for AC (Mbps): " 
			<< totalACthroughput.at(i) << endl;;

	}

	cout << "\n2. Total Collisions: " << totalCol << endl;
	cout << "2.1 Total Internal Collisions: " << totalIntCol << endl;
	cout << "2.2 Summing collision metrics from all stations" << endl;
	for(int i = 0; i < AC; i++)
	{
		cout << "\tAC " << i << ": " << totalACCol.at(i) / Nodes << ". Collisions AC / Total Transmitted." << endl;
		cout << "\tInternal Collisions: " << totalIntACCol.at(i) << endl << endl;
	}

	cout << "\n3. Total Retransmissions: " << totalRetransmissions << ". Retransmitted / Transmitted ratio: " 
		<< totalRetransmissions / totalTx << endl;

	for(int i = 0; i < AC; i++)
	{
		cout << "\tAC " << i << ": " << totalACRet.at(i) << ". Retransmitted AC / Total Transmitted: " 
			<< totalACRet.at(i) / totalTx << endl;

	}

	cout << "\n4. Total Dropped packets due to RET " << totalDropped << ". Dropped / SxSent ratio: "
		<< totalDropped / totalSentPackets << endl;

	for(int i = 0; i < AC; i++)
	{
		cout << "\tAC " << i << ": " << droppedAC.at(i) << ". Dropped AC / SxSent ratio: "
			<< droppedAC.at(i) / totalSentPackets << endl;
	}

	cout << "\n5. Overall erased packets failure index (at least one should be 1). In sat: " << 
	( (totalIncommingPackets - totalErasedPackets) / totalRemainingPackets ) << ". Non-sat: " << 
	totalIncommingPackets / (totalErasedPackets + totalRemainingPackets) <<  endl;
	
	
	


	cout << "\n6. Overall Fairness: " << (double)((pow(overallFairnessNum,2))/(Nodes*(overallFairnessDenom))) << endl;

	for(int i = 0; i < AC; i++)
	{
		cout << "\tAC " << i << " fairness: " << fairnessAC.at(i) << endl;
	}

	cout << "\n7. Average time between successful transmissions for each AC:" << endl;

	for(int i = 0; i < AC; i++)
	{
		cout << "\tAC " << i << ": " << avgTimeBetweenACSxTx.at(i)/Nodes << endl;
	}

	cout << "\n8. Number of times the queue empties for each AC:" << endl;

	for(int i = 0; i < AC; i++)
	{
		cout << "\tAC " << i << ": " << qEmpties.at(i) << endl;
	}

	cout << "\n9. Packets generated: " << totalSource << endl;

	for(int i = 0; i < AC; i++)
	{
		cout << "\tAC " <<  i << ": " << sourceForAC.at(i) << endl;
	}

	

	cout << "\n10. Proportion of EDCA nodes: " << percentageEDCA_*100 << "%" << endl;
	
	
	
	cout << "\tThroughput:" << endl;
	cout << "\tEDCA nodes (" << EDCAnodes << "): " << totalThroughputEDCA << endl;
	for(int i = 0; i < AC; i++)
	{
		cout << "\t\tAC " << i << ": " << totalACthroughputEDCA.at(i) << endl;
	}
	cout << "\t-EDCA nodes (" << EDCAnodes << "): " << totalThroughputECA << endl;
	for(int i = 0; i < AC; i++)
	{
		cout << "\t\tAC " << i << ": " << totalACthroughputECA.at(i) << endl;
	}

	cout << "\tAvg. Time between successful transmissions: " << endl;
	cout << "\t-EDCA nodes (" << EDCAnodes << "): " << endl;
	for(int i = 0; i < AC; i++)
	{
		cout << "\t\tAC " << i << ": " << (double)avgTimeBetweenACSxTxEDCA.at(i)/EDCAnodes << endl;
	}
	cout << "\t-CSMA/ECA_qos nodes (" << ECAnodes << "): " << endl;
	for(int i = 0; i < AC; i++)
	{
		cout << "\t\tAC " << i << ": " << (double)avgTimeBetweenACSxTxECA.at(i)/ECAnodes << endl;
	}

	cout << endl;
	cout << "\tFraction of collisions: " << endl;
	cout << "\t-EDCA nodes (" << EDCAnodes << "): " << endl;
	for(int i = 0; i < AC; i++)
	{
		cout << "\t\tAC " << i << ": " << (double)(totalACColEDCA.at(i)/EDCAnodes) << endl;
	}
	cout << "\t-CSMA/ECA_qos nodes (" << ECAnodes << "): " << endl;
	for(int i = 0; i < AC; i++)
	{
		cout << "\t\tAC " << i << ": " << (double)(totalACColECA.at(i)/ECAnodes) << endl;
	}		

}int main(int argc, char *argv[])
{
	int MaxSimIter;
	double SimTime;
	int NumNodes;
	int PacketLength;
	double Bandwidth;
	int Batch;
	int Stickiness;
	int ECA;
	int fairShare;
	float channelErrors;
	float slotDrift;
	float percentageEDCA;
	int maxAggregation;
	int howManyACs;
	int simSeed;
	
	if(argc < 12) 
	{
		if(argv[1])
		{
			string word = argv[1];
			string help ("--help");
			string helpShort ("-h");
			if((word.compare(help) == 0) || (word.compare(helpShort) == 0)){
				cout << endl;
				cout << "------------" << endl;
				cout << "Cheatsheet:" << endl;
				cout << "------------" << endl;
				cout << "(0)./XXXX (1)SimTime (2)NumNodes (3)PacketLength (4)Bandwidth (5)Batch (6)ECA (7)Stickiness (8)fairShare (9)channelErrors (10)slotDrift (11)percentageOfEDCA (12)maxAggregation (13)simSeed" << endl << endl;;
				cout << "0) ./XXX: Name of executable file" << endl;
				cout << "1) SimTime: simulation time in seconds" << endl;
				cout << "2) NumNodes: number of contenders" << endl;
				cout << "3) PacketLength: length of the packet in bytes" << endl;
				cout << "4) Bandwidth: number of bits per second generated by the source. With 802.11n and EDCA, 10e6 < is considered an unsaturated environment." << endl;
				cout << "5) Batch: how many packets are put in the contenders queue. Used to simulate burst traffic. Usually set to 1" << endl;
				cout << "6) EDCA: using a deterministic backoff after successful transmissinos (0=no, 1=yes)" << endl;
				cout << "7) Stickiness: a deterministic backoff is used for this many number of collisions" << endl;
				cout << "8) FairShare: nodes at backoff stage k, attempt the transmission of 2^k packets (0=off, 1=on)" << endl;
				cout << "9) ChannelErrors: channel errors probability [0,1]" << endl;
				cout << "10) SlotDrift: probability of miscounting passing empty slots [0,1]" << endl;
				cout << "11) PercetageEDCA: percentage of nodes running EDCA. Used to simulate CSMA/ECA and CSMA/CA mixed scenarios [0,1]" << endl;
				cout << "12) MaxAggregation: nodes use maximum aggregation when attempting transmission (0=off, 1=on)" << endl;
				cout << "13) howManyACs: you can start simulations with 1 to 4 ACs" << endl;
				cout << "14) SimSeed: simulation seed used to generate random numbers. If testing results, repeat simulations with different seeds everytime" << endl << endl;
				return(0);
			}else
			{
				cout << endl;
				cout << "Alert: Unintelligible command" << endl;
				cout << "Use the parameter --help or -h to display the available settings" << endl << endl;
				return(0);
			}
		}else
		{
			cout << "Executed with default values shown below" << endl;
			cout << "./XXXX SimTime [10] NumNodes [2] PacketLength [1024] Bandwidth [65e6] Batch [1] ECA [0] Stickiness [0] fairShare [0] channelErrors [0] slotDrift [0] percentageOfEDCA [1] maxAggregation [0] howManyACs [0] simSeed [0]" << endl;
			MaxSimIter = 1;
			SimTime = 10;
			NumNodes = 30;
			PacketLength = 1024;
			Bandwidth = 65e6;
			Batch = 1; 
			Stickiness = 0; 
			ECA = 0;	
			fairShare = 0; 
			channelErrors = 0; 
			slotDrift = 0; 
			percentageEDCA = 1; 
			maxAggregation = 0;
			howManyACs = 4; 
			simSeed = 2234; 
		}
	}else
	{
		MaxSimIter = 1;
		SimTime = atof(argv[1]);
		NumNodes = atoi(argv[2]);
		PacketLength = atoi(argv[3]);
		Bandwidth = atof(argv[4]);
		Batch = atoi(argv[5]); 
		ECA = atoi(argv[6]); 
		Stickiness = atoi(argv[7]); 
		fairShare = atoi(argv[8]); 
		channelErrors = atof(argv[9]); 
		slotDrift = atof(argv[10]); 
		percentageEDCA = atof(argv[11]); 
		maxAggregation = atoi(argv[12]); 
		howManyACs = atoi(argv[13]); 
		if(howManyACs == 0){
			howManyACs = 1;	
		}else if (howManyACs > 4)
		{
			howManyACs = 4;
		}
		simSeed = atof(argv[14]); 
	}

	printf("\n####################### Simulation (Seed: %d) #######################\n",simSeed);
	if(Stickiness > 0)
	{
		if(ECA > 0)
		{
			if(fairShare > 0)
			{
				cout << "####################### Full ECA #######################" << endl;
			}else
			{
				cout << "################### ECA + hysteresis ###################" << endl;
			}
		}else
		{
			cout << "###################### Basic ECA ######################" << endl;
		}
	}else
	{
		cout << "####################### CSMA/CA #######################" << endl;
	}
	
	if(percentageEDCA > 0) cout << "####################### Mixed setup " << percentageEDCA*100 << "% EDCA #######################" << endl;
		
	compcxx_SlottedCSMA_9 test;

	
	
	test.Seed = simSeed;
		
	test.StopTime(SimTime);

	test.Setup(MaxSimIter,NumNodes,PacketLength,Bandwidth,Batch,Stickiness, ECA, fairShare, channelErrors, slotDrift, percentageEDCA, maxAggregation, howManyACs, simSeed);
	
	test.Run();


	return(0);
};
