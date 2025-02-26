#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

#!/usr/bin/env bats

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "cd with no arguments does nothing" {
    run ./dsh <<EOF
cd
EOF

    [ "$status" -eq 0 ]
}

@test "Example: random command does nothing" {
    run ./dsh <<EOF
randomthing
EOF

    [ "$status" -eq 0 ]
}

@test "pipe with multiple commands works correctly" {
    run "./dsh" <<EOF
echo a | tr 'a' 'b' | tr 'b' 'c'
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="cdsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "pipe with no spaces around pipes works correctly" {
    run "./dsh" <<EOF
echo a|tr 'a' 'z'
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="zdsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Builtin cd with absolute path changes directory" {
    run "./dsh" <<EOF
cd /
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="/dsh3>dsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Pipe character inside quotes is preserved" {
    run "./dsh" <<EOF
echo "|" | grep "\|"
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="|dsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Command with extra spaces around pipes executes correctly" {
    run "./dsh" <<EOF
echo hello    |   tr 'h' 'H'
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="Hellodsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "pipe with maximum segments (8 commands)" {
    run "./dsh" <<EOF
echo 1 | echo 2 | echo 3 | echo 4 | echo 5 | echo 6 | echo 7 | echo 8
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="8dsh3>dsh3>cmdloopreturned0"
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "pipe with more than maximum segments (9 commands)" {
    run "./dsh" <<EOF
echo 1 | echo 2 | echo 3 | echo 4 | echo 5 | echo 6 | echo 7 | echo 8 | echo 9
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="dsh3>error:pipinglimitedto8commandsdsh3>cmdloopreturned-2"
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "echo pipe in double quotes works correctly" {
    run "./dsh" <<EOF
echo "|"
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="|dsh3>dsh3>cmdloopreturned0"
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "pipe with built-in cd and external command works correctly" {
    run "./dsh" <<EOF
echo hello | cd / | echo world
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="worlddsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "pipe with only spaces between the pipe both sides works correctly" {
    run "./dsh" <<EOF
echo hello     |    echo world
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="worlddsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "pipe with mixed quoting and spaces works correctly" {
    run "./dsh" <<EOF
echo "a b" | tr -d ' '
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="abdsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "pipe with multi-line output works correctly" {
    run "./dsh" <<EOF
printf "line1\nline2\n" | grep line
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="line1line2dsh3>dsh3>cmdloopreturned0"
    
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}