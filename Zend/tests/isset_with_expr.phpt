--TEST--
isset() with arbitrary expressions
--FILE--
<?php

function getNull() { return null; }
function getFalse() { return false; }

var_dump(isset(null));
var_dump(isset(false));
var_dump(isset(new stdClass));

var_dump(isset(getNull()));
var_dump(isset(getFalse()));

var_dump(isset(1 + 1));
var_dump(isset((unset) (1 + 1)));
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
