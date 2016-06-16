--TEST--
Bug #39017 (foreach(($obj = new myClass) as $v); echo $obj; segfaults)
--FILE--
<?php

class A
{
}
function fn2002175578()
{
    foreach ($a = (object) new A() as $v) {
    }
    var_dump($a);
    // UNKNOWN:0

}
fn2002175578();
--EXPECTF--
object(A)#%d (0) {
}
