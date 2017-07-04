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
function fn6333201()
{
    var_dump(new Test());
}
fn6333201();
--EXPECT--
object(Test)#1 (1) {
  [0]=>
  *RECURSION*
}
