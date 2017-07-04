--TEST--
Force pass-by-reference to __call 
--FILE--
<?php

class C
{
    function __call($name, $values)
    {
        $values[0][0] = 'changed';
    }
}
function fn2079925421()
{
    $a = array('original');
    $b = array('original');
    $hack =& $b[0];
    $c = new C();
    $c->f($a);
    $c->f($b);
    var_dump($a, $b);
}
fn2079925421();
--EXPECTF--
array(1) {
  [0]=>
  string(8) "original"
}
array(1) {
  [0]=>
  &string(7) "changed"
}

