from flask import Flask, jsonify
from flask_cors import CORS
import requests
import threading
import time

app = Flask(__name__)
CORS(app)  # Enable CORS for cross-origin requests

# Global variables to hold the contest number and the latest result.
# The result will be a plain dictionary.
result = {"message": "No data yet"}
contest_no = None

# A constant used in calculating penalties.
WRONG_SUBMISSION_PENALTY = 20

@app.route('/leaderboard/<int:contest>')
def send_leaderboard(contest):
    global contest_no
    # Update the contest number from the URL parameter.
    contest_no = contest
    # Return the current result as a JSON response.
    return jsonify(result)

def get_participants(contest_no):
    """
    Fetch data from VJudge and process it to build a participants dictionary.
    Uses the full logic provided for score and penalty calculations.
    """
    start = time.time()
    # Fetch the contest ranking data from VJudge.
    response = requests.get(f"https://vjudge.net/contest/rank/single/{contest_no}", timeout=10)
    data = response.json()
    submissions = data['submissions']
    participants_vjudge = data['participants']

    participants = {}
    for submission in submissions:
        pid = submission[0]
        problem_index = submission[1]
        verdict = submission[2]
        time_submission = submission[3]
        
        # If the participant is already in our dictionary
        if pid in participants:
            # If a record for this problem already exists
            if participants[pid]['problem'][problem_index] is not None:
                participants[pid]['problem'][problem_index]['submissions'] += 1

                if participants[pid]['problem'][problem_index]['verdict'] == 1:
                    # If a correct submission exists and the new submission is faster, add a penalty
                    if participants[pid]['problem'][problem_index]['time'] > time_submission:
                        participants[pid]['penalty'] += int(WRONG_SUBMISSION_PENALTY)
                    else:
                        continue
                # If this submission is correct or there is additional scoring info (len>4)
                elif verdict == 1 or len(submission) > 4:
                    participants[pid]['problem'][problem_index]['verdict'] = verdict
                    participants[pid]['problem'][problem_index]['time'] = time_submission
                    participants[pid]['penalty'] += int(time_submission / 60)
                    # Add penalty for previous wrong submissions
                    participants[pid]['penalty'] += int(
                        (participants[pid]['problem'][problem_index]['submissions'] - 1) * WRONG_SUBMISSION_PENALTY
                    )
                    participants[pid]['score'] += verdict
                    # If additional scoring info is present, add it
                    if len(submission) > 4:
                        participants[pid]['score'] += round(submission[4] / submission[5], 1)
                else:
                    participants[pid]['problem'][problem_index]['verdict'] = verdict

            # If no record exists for this problem yet.
            else:
                participants[pid]['problem'][problem_index] = {
                    'submissions': 1,
                    'verdict': verdict,
                    'time': int(time_submission)
                }
                participants[pid]['score'] += verdict
                if verdict == 1 or len(submission) > 4:
                    if len(submission) > 4:
                        participants[pid]['score'] += round(submission[4] / submission[5], 1)
                    participants[pid]['penalty'] += int(time_submission / 60)
        else:
            # Create a new participant record.
            participants[pid] = {}
            # Get the participant name from the VJudge data.
            participants[pid]['name'] = participants_vjudge.get(str(pid), ['Unknown'])[0]
            participants[pid]['score'] = 0
            participants[pid]['penalty'] = 0
            # Create a list to hold data for 7 problems (adjust if needed).
            participants[pid]['problem'] = [None, None, None, None, None, None, None]
            # Set the submission info for the current problem.
            participants[pid]['problem'][problem_index] = {
                'submissions': 1,
                'verdict': verdict,
                'time': int(time_submission)
            }
            if verdict == 1:
                participants[pid]['score'] += 1
                participants[pid]['penalty'] += int(time_submission / 60)
    
    print("Processed in", round(time.time() - start, 2), "seconds")
    return participants

def leaderboard_updater():
    """
    Background thread that updates the global `result` every 5 seconds.
    """
    global result, contest_no
    while True:
        if contest_no is not None:
            try:
                # Use get_participants to update the result.
                result = get_participants(contest_no)
            except Exception as e:
                result = {"error": str(e)}
        # Update every 5 seconds.
        time.sleep(5)

if __name__ == '__main__':
    # Start the leaderboard updater thread as a daemon.
    updater_thread = threading.Thread(target=leaderboard_updater, daemon=True)
    updater_thread.start()
    
    # Run the Flask app.
    app.run(host='0.0.0.0', port=5000)
