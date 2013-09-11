--TEST--
Return typing test
--INI--
error_reporting=E_ALL
--FILE--
<?php

function demo() : array {
    return 1;
}

function demo2() : stdClass {
    return array();
}

class Test {
}

function demo3() : Test {
    return new stdClass();
}

var_dump(demo());
var_dump(demo2());
var_dump(demo3());

$demo = function() : array {
    return new stdClass();
};

var_dump($demo());

function demo4(callable $a) : Test {
    return $a();
}
var_dump(demo4(function() : Test {
    return new Test();

}));

$demo2 = function() use($demo) : array {
    return $demo();
};

var_dump($demo2());

--EXPECTF--
[E_RECOVERABLE_ERROR] Return type mismatch, expected array got integer
