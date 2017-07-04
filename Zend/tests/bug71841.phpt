--TEST--
Bug #71841 (EG(error_zval) is not handled well)
--INI--
error_reporting=0
--FILE--
<?php

function fn395335631()
{
    $z = unserialize('O:1:"A":0:{}');
    var_dump($z->e .= 0);
    var_dump(++$z->x);
    var_dump($z->y++);
    $y = array(PHP_INT_MAX => 0);
    var_dump($y[] .= 0);
    var_dump(++$y[]);
    var_dump($y[]++);
}
fn395335631();
--EXPECT--
NULL
NULL
NULL
NULL
NULL
NULL
