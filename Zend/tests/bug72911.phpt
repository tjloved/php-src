--TEST--
Bug #72911 (Memleak in zend_binary_assign_op_obj_helper)
--FILE--
<?php

function fn1831274863()
{
    $a = 0;
    $b = $a->b->i -= 0;
    var_dump($b);
}
fn1831274863();
--EXPECTF--
Warning: Attempt to modify property of non-object in %sbug72911.php on line %d

Warning: Attempt to assign property of non-object in %sbug72911.php on line %d
NULL
