#include "gtest/gtest.h"

#include "ATen/ATen.h"
#include "test_seed.h"

#include<iostream>
using namespace std;
using namespace at;

void trace() {
  Tensor foo = rand({12,12});

  // ASSERT foo is 2-dimensional and holds floats.
  auto foo_a = foo.accessor<float,2>();
  float trace = 0;

  for(int i = 0; i < foo_a.size(0); i++) {
    trace += foo_a[i][i];
  }

  EXPECT_FLOAT_EQ(foo.trace().toCFloat(), trace);
}

// TEST_CASE( "atest", "[]" ) {
TEST(atest, atest) {
  manual_seed(123, at::kCPU);
  manual_seed(123, at::kCUDA);

  auto foo = rand({12,6});
  EXPECT_EQ(foo.data<float>(), foo.toFloatData());

  EXPECT_EQ(foo.size(0), 12);
  EXPECT_EQ(foo.size(1), 6);

  foo = foo+foo*3;
  foo -= 4;

  Scalar a = 4;
  float b = a.to<float>();
  EXPECT_EQ(b, 4);

  foo = (foo*foo) == (foo.pow(3));
  foo =  2 + (foo+1);
  //foo = foo[3];
  auto foo_v = foo.accessor<uint8_t,2>();

  for(int i = 0; i < foo_v.size(0); i++) {
    for(int j = 0; j < foo_v.size(1); j++) {
      foo_v[i][j]++;
    }
  }

  EXPECT_TRUE(foo.equal(4 * ones({12, 6}, kByte)));

  trace();

  float data[] = { 1, 2, 3,
                   4, 5, 6};

  auto f = CPU(kFloat).tensorFromBlob(data, {1,2,3});
  auto f_a = f.accessor<float,3>();

  EXPECT_EQ(f_a[0][0][0], 1.0);
  EXPECT_EQ(f_a[0][1][1], 5.0);

  EXPECT_EQ(f.strides()[0], 6);
  EXPECT_EQ(f.strides()[1], 3);
  EXPECT_EQ(f.strides()[2], 1);
  EXPECT_EQ(f.sizes()[0], 1);
  EXPECT_EQ(f.sizes()[1], 2);
  EXPECT_EQ(f.sizes()[2], 3);

  // TODO(ezyang): maybe do a more precise exception type.
  ASSERT_THROW(f.resize_({3,4,5}), std::exception);
  {
    int isgone = 0;
    {
      auto f2 = CPU(kFloat).tensorFromBlob(data, {1,2,3}, [&](void*) {
        isgone++;
      });
    }
    EXPECT_EQ(isgone, 1);
  }
  {
    int isgone = 0;
    Tensor a_view;
    {
      auto f2 = CPU(kFloat).tensorFromBlob(data, {1,2,3}, [&](void*) {
        isgone++;
      });
      a_view = f2.view({3,2,1});
    }
    EXPECT_EQ(isgone, 0);
    a_view.reset();
    EXPECT_EQ(isgone, 1);
  }

  if(at::hasCUDA()) {
    int isgone = 0;
    {
      auto base = CUDA(kFloat).tensor({1,2,3});
      auto f2 = CUDA(kFloat).tensorFromBlob(base.data_ptr(), {1,2,3}, [&](void*) {
        isgone++;
      });
    }
    EXPECT_EQ(isgone, 1);
  }
}
