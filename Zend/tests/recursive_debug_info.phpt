--TEST--
Test recursive __debugInfo() method
--FILE--
<?php

class Test
{
    public function __debugInfo()
    {
        return [$this];
    }
}
function fn694376484()
{
    var_dump(new Test());
}
fn694376484();
--EXPECT--
object(Test)#1 (1) {
  [0]=>
  *RECURSION*
}
