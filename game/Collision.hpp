#pragma once

struct CollisionBody2D;


struct CollisionDetectionStrategy2D {
    virtual ~CollisionDetectionStrategy2D() {}

    virtual bool is_collide(CollisionBody2D const&, CollisionBody2D const&) = 0;
    virtual void resolve_collision(CollisionBody2D&, CollisionBody2D&) = 0;
};


struct CollisionDetectionStrategy2DUseAABB final : public CollisionDetectionStrategy2D {
    bool is_collide(CollisionBody2D const&, CollisionBody2D const&) override;
    void resolve_collision(CollisionBody2D&, CollisionBody2D&) override;
};

struct CollisionDetectionStrategy2DUseRayCasting final : public CollisionDetectionStrategy2D {
    bool is_collide(CollisionBody2D const&, CollisionBody2D const&) override;
    void resolve_collision(CollisionBody2D&, CollisionBody2D&) override;
};

struct DistanceDetectorStrategy2D {
    virtual ~DistanceDetectorStrategy2D() {}

    virtual float distance_between(CollisionBody2D const&, CollisionBody2D const&) = 0;
};

struct DistanceDetectorStrategy2DUsePythagorean final : public DistanceDetectorStrategy2D {
    virtual float distance_between(CollisionBody2D const&, CollisionBody2D const&) override;
};