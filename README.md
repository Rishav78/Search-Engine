# Search_Engine

## This project include 3 files
<ul>
  <li>Crawler</li>
  <li>Indexer</li>
  <li>Query Engine</li>
</ul>

<dl>
  <dt>Crawler</dt>
  <dd>
    Crawler jump from one url to another take it's HTML code save it into the local file. We provide the root url and crawler jump to that url, find next valid link from the html and jump to that url and this process continue until he reach a certain depth.
    
  </dd>
  <dt> Indexer </dt>
  <dd>
    Indexer take all the html file and fetch all the keyswords(words in TITLE, HEADING, META Tag) and index then in a local file.
  </dd>
  <dt> Query Engine </dt>
  <dd>
    Query Engine take the query from the user and show the relevent files. It take the indexed file and when user perform a query it search all the keywords that user enter in the indexed file and according to the priority it show the file from higher to lower priority
  </dd>
</dl>

## crawler command
#### `./a.out (https://www.example.com) (depth eg 4,5) (path of current dir)`
## indexer command
#### `./a.out (path to current dir)`
## queryEngine command
#### `./a.out (path to current dir)`
