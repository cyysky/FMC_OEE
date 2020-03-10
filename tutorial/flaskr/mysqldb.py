import mysql.connector
import traceback

def get_db_cursor():
    mydb = mysql.connector.connect(
      host="localhost",
      user="root",
      passwd="123456",
      database="db"
    )
    mycursor = mydb.cursor(dictionary=True)
    
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
        
        mycursor.execute("SELECT m.id, machine_name, machine_key, location, m.active, alive, active_job_id, job, mold_name, quantity, bag, material, cavity, date_time, user_id FROM machine as m "
        "left join job as j on j.id = m.active_job_id;")
        
        myresult = mycursor.fetchall()
        
        return myresult
       
    except:
        traceback.print_exc() 
        close_db(mydb)      

def get_jobs(active=True):

    try:    
        mydb,mycursor = get_db_cursor()     
        
        mycursor.execute("SELECT * FROM machine as m "
        "left join job as j on j.id = m.active_job_id;")
        
        myresult = mycursor.fetchall()
        
        return myresult
       
    except:
        traceback.print_exc() 
        close_db(mydb)     
         

def get_machines2():

    try:    
        mydb,mycursor = get_db_cursor()     
        
        mycursor.execute("SELECT * FROM machine as m "
        "left join job as j on j.id = m.active_job_id;")
        
        row_headers=[x[0] for x in mycursor.description] #this will extract row headers
        
        myresult = mycursor.fetchall()
        
        json_data=[]
        for result in myresult:
            json_data.append(dict(zip(row_headers,result)))        
        result = json.dumps(json_data, indent=4, sort_keys=True, default=str)
        return jsonify(result)
        
        #return myresult
       
    except:
        traceback.print_exc() 
        close_db(mydb)            