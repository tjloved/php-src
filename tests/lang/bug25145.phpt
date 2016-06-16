--TEST--
Bug #25145 (SEGV on recpt of form input with name like "123[]")
--GET--
123[]=SEGV
--FILE--
<?php

function fn705464400()
{
    var_dump($_REQUEST);
    echo "Done\n";
}
fn705464400();
--EXPECT--
array(1) {
  [123]=>
  array(1) {
    [0]=>
    string(4) "SEGV"
  }
}
Done
