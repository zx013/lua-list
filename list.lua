

function test_list_01()
    local l = []
    for i = 1, 10 do
        l[#l + 1] = i
    end

    for i = 1, 10 do
        assert(l[i] == i)
    end
end

test_list_01()
