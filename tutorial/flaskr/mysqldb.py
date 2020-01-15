import mysql.connector
import traceback

def get_db_cursor():
    mydb = mysql.connector.connect(
      host="localhost",
      user="root",
      passwd="123456",
      database="db"
    )
    mycursor = mydb.cursor()
    
    return mydb,mycursor

def close_db(mydb):
    try:
        mydb.close()
        print("MySql Connection Closed")
    except:
        traceback.print_exc()

def print_connector():
    mydb = mysql.connector.connect(
      host="localhost",
      user="root",
      passwd="123456"
    )
    print(mydb)
    
def insert_machine(machineName,machineKey):
    try:    
        mydb,mycursor = get_db_cursor()     
        
        sql = "INSERT INTO machine (machine_name,machine_key) VALUES (%s, %s)"
        val = (machineName, machineKey)
        mycursor.execute(sql, val)

        mydb.commit()
        
        print(mycursor.rowcount, "record inserted.")       
    except:
        traceback.print_exc() 
        close_db(mydb) 

    
def get_machines():

    try:    
        mydb,mycursor = get_db_cursor()     
        
        mycursor.execute("SELECT * FROM machine as m "
        "left join job as j on j.id = m.active_job_id;")
        
        myresult = mycursor.fetchall()
        
        return myresult
       
    except:
        traceback.print_exc() 
        close_db(mydb)            