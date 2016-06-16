--TEST--
errmsg: arrays are not allowed in class constants
--FILE--
<?php

class test
{
    const TEST = array(1, 2, 3);
}
function fn1966198241()
{
    var_dump(test::TEST);
    echo "Done\n";
}
fn1966198241();
--EXPECTF--	
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
Done
