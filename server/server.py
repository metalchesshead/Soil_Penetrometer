import os, json, pytest, subprocess, sys, csv, shutil
from flask import Flask
from datetime import datetime
from flask import render_template, Blueprint, request, jsonify, session, url_for, redirect, session
from flask.logging import default_handler
#from flask_cors import CORS, cross_origin
#from flask_session import Session
#from redis import Redis

app = Flask(__name__)

# class Thing(object):
        # def __init__(self, name):
            # self.name = name
SAVE_DIR    = os.path.dirname(os.path.abspath(__file__))
times   = []
forces  = []
array_size = 10
@app.route("/")
def home():

    print("ccff")
    # d=open('data.json', 'r')
    # e = json.loads(d.read())
    # d.close
    # print(e)
    # subprocess.Popen("test.py {e}", shell=True)
    
    #with open("data.json", "r") as f:
    #    e =(json.load(1))
        #f.close()
    #    print(e)
        
    #f = open("data.json", 'r')
    #e = f.read()
    #f.close()
    x = 0
    #with open("data.json", 'r') as f:
        
    #    for line in f:
    #        e = f.read
            #if json.dumps(e.get("uu")) != "null":
            #    print(float(json.dumps(data.get("uu")).strip('""')))
            #    #x = x+int(e.get("uu"))
    #        x = x+5
  #  print(x)
  #  f.close
   # open('data.json', 'w').close()
   # print(e)
    
    g = open("getdata.json", 'w')
    #json.dumps({"getdata":"0"})
    g.write('{"getdata":"0"}')
    g.close
    #    f.write('\n')
    
    return render_template('indexq.html')
   
    
@app.route('/tile', methods=["GET", "POST"])
def tile_stuff():
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    print(timestamp)
    

    # if json.dumps(data.get("uu")) != "null":
        # print(float(json.dumps(data.get("uu")).strip('""')))
        
    # with open('data.json', 'a') as f:
       # json.dump(data, f)
       # f.write('\n')  
    g = open("getdata.json", 'r')
    e = json.loads(g.read())
    g.close

    if e.get("getdata") == '1':
        times.clear()
        forces.clear()
        csv_name  = os.path.join(SAVE_DIR, f"penetrometer_.csv {timestamp}")
        data = request.get_json(force=True)
        print('wsss')
        line = data.get("uu")
        print(line)
        parts = line.split(",")
        print(parts)
        for part in parts[1::2]:
            raw_adc = int(part)
            force_kg = (raw_adc)

            force_zeroed = abs(force_kg)  # abs handles either load cell direction
        #now = time.time()
        #t = now - t_start
        #times.append(t)
            forces.append(force_zeroed)
        for time in parts[::2]:
            times.append(time)
        #open('getdata.json', 'w').close()
        # with open('data.json', 'a') as f:
            # json.dump(data, f)
            # f.write('\n') 
        with open(csv_name, "w", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(["Time (s)", "Force (kg)"])
            writer.writerows(zip(times, forces))
            print(f"CSV saved   → {csv_name}\n")
            f.close
        g = open("getdata.json", 'w')
        g.write('{"getdata":"0"}')
        g.close
        foldername = timestamp
        os.makedirs(foldername)
        shutil.move(csv_name, foldername)
        shutil.move("coordinates.txt", foldername)
        
        return '1'
    else:
        return '0'
    
    #return 'hello'
    
@app.route("/sensor", methods=["GET", "POST"])
def send_sensor_values():
    data = request.get_json(force=True)
    #print(data)
    f = open("data.json", 'r')
    #e = json.loads(f.read())
    e = f.read()
    f.close
    #open('data.json', 'w').close()
    print(e)
    return jsonify(e)

@app.route("/collect", methods=["GET", "POST"])
def collect():
    data = request.get_json(force=True)
    print(data)
    if data.get("getdata") == '1':
        g = open("getdata.json", 'w')
        g.write('{"getdata":"1"}')
        g.close
        h = open("coordinates.txt", 'w')
        h.write(data.get("location"))
        h.close
        
    else:
        print('error')
    return jsonify(data)
