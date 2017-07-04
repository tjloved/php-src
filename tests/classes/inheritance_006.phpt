--TEST--
Private property inheritance check
--FILE--
<?php

class A
{
    private $c;
}
class B extends A
{
    private $c;
}
class C extends B
{
}
function fn2045661127()
{
    var_dump(new C());
}
fn2045661127();
--EXPECTF--
object(C)#%d (2) {
  ["c":"B":private]=>
  NULL
  ["c":"A":private]=>
  NULL
}
