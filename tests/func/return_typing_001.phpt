--TEST--
Return typing test
--FILE--
<?php

function demo() : array {
    return array();
}

function demo2() : stdClass {
    return new stdClass();
}

class Test {
}

function demo3() : Test {
    return new Test();
}

var_dump(demo());
var_dump(demo2());
var_dump(demo3());

$demo = function() : array {
    return array();
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

--EXPECT--
array(0) {
}
object(stdClass)#1 (0) {
}
object(Test)#1 (0) {
}
array(0) {
}
object(Test)#3 (0) {
}
array(0) {
}