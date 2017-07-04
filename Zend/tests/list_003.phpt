--TEST--
list() with non-array
--FILE--
<?php

function fn857025444()
{
    list($a) = NULL;
    list($b) = 1;
    list($c) = 1.0;
    list($d) = 'foo';
    list($e) = (print '');
    var_dump($a, $b, $c, $d, $e);
}
fn857025444();
--EXPECT--
NULL
NULL
NULL
NULL
NULL
