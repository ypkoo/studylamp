# install requests library first (pip install requests, or easy_install requests)

import requests
import json

URL_BASE = 'https://montanaflynn-dictionary.p.mashape.com/define?word='

def search():

    while True:
        search_word = raw_input('word? ')
        url = URL_BASE + search_word

        response = requests.get(url,
          headers={
            "X-Mashape-Key": "gIM6RaCA7xmshLUqu3z4AIGwZI3kp1XNa1NjsnBrbY45qbnTeG",
            "Accept": "application/json"
          }
        )

        raw_text = response.text
        text = raw_text.encode('utf8')
        data = json.loads(text)

        for definition in data['definitions']:
            print definition['text']
            print ''
