--TEST--
Bug #69788: Malformed script causes Uncaught Error in php-cgi, valgrind SIGILL
--FILE--
<?php

function fn379407597()
{
    [t . []];
}
fn379407597();
--EXPECTF--
Notice: Array to string conversion in %s on line %d

Notice: Use of undefined constant t - assumed 't' in %s on line %d
