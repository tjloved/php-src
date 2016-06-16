--TEST--
Bug #27598 (list() array key assignment causes HUGE memory leak)
--FILE--
<?php

function fn1620073927()
{
    list($out[0]) = array(1);
    var_dump($out);
}
fn1620073927();
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
