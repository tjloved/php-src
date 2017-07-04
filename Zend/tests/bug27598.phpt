--TEST--
Bug #27598 (list() array key assignment causes HUGE memory leak)
--FILE--
<?php

function fn1544324233()
{
    list($out[0]) = array(1);
    var_dump($out);
}
fn1544324233();
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
