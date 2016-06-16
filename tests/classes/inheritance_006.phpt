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
function fn296936641()
{
    var_dump(new C());
}
fn296936641();
--EXPECTF--
object(C)#%d (2) {
  [%u|b%"c":%u|b%"B":private]=>
  NULL
  [%u|b%"c":%u|b%"A":private]=>
  NULL
}
